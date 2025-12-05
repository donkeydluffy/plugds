
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QGuiApplication>
#include <QIcon>
#include <QJsonDocument>
#include <QMainWindow>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTimer>
#include <QTranslator>
#include <QtGlobal>
#include <chrono>

#include "SplashScreen.h"
#include "extsystem/Component.h"
#include "extsystem/IComponentManager.h"

auto constexpr kSplashscreenTimeout = 3000;

/**
 * 日志级别控制：
 *
 * 命令行参数用法：
 *   ./executable --log-level debug    # 显示所有调试信息（默认）
 *   ./executable --log-level info     # 只显示重要信息
 *   ./executable --log-level warn     # 只显示警告和错误
 *   ./executable --log-level err    # 只显示错误信息
 *
 * 日志级别说明：
 *   - trace: 最详细的跟踪信息
 *   - debug: 调试信息（开发用）
 *   - info: 重要的业务信息（生产环境推荐）
 *   - warn: 警告信息，但程序可正常运行
 *   - err: 错误信息，可能影响功能
 *   - critical: 严重错误，程序可能无法继续
 */
int main(int argc, char** argv) {
#if (QT_VERSION_MAJOR < 6)
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif
  QApplication application_instance(argc, argv);

  // 解析命令行参数
  QStringList args = QApplication::arguments();
  spdlog::level::level_enum log_level = spdlog::level::debug;  // 默认debug级别

  for (int i = 1; i < args.size(); ++i) {
    if (args[i] == "--log-level" && i + 1 < args.size()) {
      QString level_str = args[i + 1].toLower();
      if (level_str == "trace") {
        log_level = spdlog::level::trace;
      } else if (level_str == "debug") {
        log_level = spdlog::level::debug;
      } else if (level_str == "info") {
        log_level = spdlog::level::info;
      } else if (level_str == "warn") {
        log_level = spdlog::level::warn;
      } else if (level_str == "err") {
        log_level = spdlog::level::err;
      } else if (level_str == "critical") {
        log_level = spdlog::level::critical;
      }
      break;
    }
  }

  // 初始化spdlog日志系统
  auto file_logger = spdlog::basic_logger_mt("main", "ds.log", true);
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(log_level);
  spdlog::flush_on(spdlog::level::info);

  auto app_start_time = std::chrono::high_resolution_clock::now();
  SPDLOG_INFO("Application starting... (Log level: {})", spdlog::level::to_string_view(log_level));

  qApp->setApplicationName("DefinSight");
  qApp->setOrganizationName("3d-scantech");

  QList<QTranslator*> translators;

  auto* component_loader = new sss::extsystem::ComponentLoader;

  auto application_dir = QDir(qApp->applicationDirPath());
  QString translations_path;

  if (application_dir.exists("translations")) {
    translations_path = static_cast<char>(application_dir.exists("translations"));
  }

  QDir translations_directory(translations_path);

  QStringList translation_files = translations_directory.entryList();

  for (const auto& filename : translation_files) {
    if (filename.endsWith(".qm")) {
      auto* translator = new QTranslator;

      if (translator->load(translations_path + "/" + filename)) {
        translators.append(translator);

        qApp->installTranslator(translator);
      }
    }
  }

  sss::SplashScreen* splash_screen = sss::SplashScreen::GetInstance();

  SPDLOG_INFO("Splash screen created.");

  splash_screen->show();

  auto* component_manager = sss::extsystem::IComponentManager::GetInstance();

  SPDLOG_INFO("Component manager instance obtained.");

  component_manager->AddObject(component_loader);

  // SPDLOG_DEBUG("Application started.");

  QStringList component_locations = QStringList() << "APPDIR" << "DS_COMPONENT_DIR";

  SPDLOG_INFO("Component locations: {}", component_locations.join(", ").toStdString());

  for (const auto& dir_name : component_locations) {
    if (QProcessEnvironment::systemEnvironment().contains(dir_name)) {
      auto dir_path = QProcessEnvironment::systemEnvironment().value(dir_name);
      auto components_path = dir_path + "/components";

      SPDLOG_INFO("Found component path via {}: {}", dir_name.toStdString(), components_path.toStdString());

      component_loader->AddComponents(components_path);
    }
  }

  if (application_dir.exists("components")) {
    auto components_path = application_dir.absoluteFilePath("components");

    SPDLOG_INFO("Found local component path: {}", components_path.toStdString());

    component_loader->AddComponents(components_path);
  }
  QString settings_path;

  if (QDir(application_dir).exists("data")) {
    settings_path = QDir::cleanPath(application_dir.absolutePath() + "/data");
  } else {
    settings_path = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(0) + "/" +
                    qApp->organizationName() + "/" + qApp->applicationName();
  }

  QString app_settings_filename = settings_path + "/appSettings.json";

  QFile settings_file(app_settings_filename);
  QVariantList disabled_components;

  settings_file.open(QFile::ReadOnly);

  if (settings_file.isOpen()) {
    auto settings = QJsonDocument::fromJson(settings_file.readAll()).toVariant();

    if (settings.isValid()) {
      auto settings_map = settings.toMap();

      if (settings_map.contains("disabledComponents")) {
        disabled_components = settings_map["disabledComponents"].toList();
      }
    }
  }

  SPDLOG_INFO("Starting component loading...");

  // 生命周期管理：
  // 1. 阶段1 (核心初始化): CoreComponent (依赖根节点) 首先初始化，设置管理器。
  // 2. 阶段2 (插件初始化): 其他插件初始化，注册上下文/提供者。
  // 3. 阶段3 (完成): 所有初始化完成后，InitialisationFinishedEvent 按相反顺序运行。
  //    CoreComponent 最后运行，构建 UI 并显示窗口。
  component_loader->LoadComponents([disabled_components](sss::extsystem::Component* component) -> bool {
    auto component_id = (component->Name() + "." + component->Vendor()).toLower();
    bool should_load = !disabled_components.contains(component_id);

    SPDLOG_INFO("Component {}: can_be_disabled={}, should_load={}", component_id.toStdString(),
                component->CanBeDisabled() ? "true" : "false", should_load ? "true" : "false");

    return should_load;
  });

  auto components = component_loader->Components();
  SPDLOG_INFO("Total components loaded: {}", components.size());

  SPDLOG_INFO("Component loading completed.");

  int exit_code;
  auto* main_window = (sss::extsystem::GetTObject<QMainWindow>());

  if (main_window != nullptr) {
    SPDLOG_INFO("Main window found: {}", (void*)main_window);

    SPDLOG_INFO("Setting window icon (Windows)");
    qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));

    SPDLOG_INFO("Starting event loop...");

    QTimer::singleShot(kSplashscreenTimeout, [=]() {
      SPDLOG_INFO("Closing splash screen...");
      splash_screen->deleteLater();
    });

    exit_code = QApplication::exec();

    auto app_shutdown_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(app_shutdown_time - app_start_time);
    SPDLOG_INFO("Event loop exited with code: {} (Total runtime: {}ms)", exit_code, total_duration.count());
  } else {
    SPDLOG_ERROR("Error: Main window not found! Application will exit.");
    exit_code = 1;
  }

  component_loader->UnloadComponents();

  SPDLOG_DEBUG("Unloading components...");

  delete component_loader;
  SPDLOG_DEBUG("Component loader deleted.");

  return exit_code;
}
