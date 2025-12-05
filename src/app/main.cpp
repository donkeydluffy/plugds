
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

#include "SplashScreen.h"
#include "extsystem/Component.h"
#include "extsystem/IComponentManager.h"

auto constexpr kSplashscreenTimeout = 3000;

int main(int argc, char** argv) {
  // 初始化spdlog日志系统
  auto file_logger = spdlog::basic_logger_mt("main", "ds.log", true);
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::info);

  SPDLOG_INFO("应用程序正在启动...");

  qApp->setApplicationName("DefinSight");
  qApp->setOrganizationName("3d-scantech");

#if (QT_VERSION_MAJOR < 6)
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif

  auto* application_instance = new QApplication(argc, argv);

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

  SPDLOG_INFO("已创建启动画面。");

  splash_screen->show();

  auto* component_manager = sss::extsystem::IComponentManager::GetInstance();

  SPDLOG_INFO("已获取组件管理器实例。");

  component_manager->AddObject(component_loader);

  // SPDLOG_DEBUG("应用程序已启动。");

  QStringList component_locations = QStringList() << "APPDIR" << "DS_COMPONENT_DIR";

  SPDLOG_INFO("组件位置: {}", component_locations.join(", ").toStdString());

  for (const auto& dir_name : component_locations) {
    if (QProcessEnvironment::systemEnvironment().contains(dir_name)) {
      auto dir_path = QProcessEnvironment::systemEnvironment().value(dir_name);
      auto components_path = dir_path + "/components";

      SPDLOG_INFO("通过 {} 找到组件路径: {}", dir_name.toStdString(), components_path.toStdString());

      component_loader->AddComponents(components_path);
    }
  }

  if (application_dir.exists("components")) {
    auto components_path = application_dir.absoluteFilePath("components");

    SPDLOG_INFO("找到本地组件路径: {}", components_path.toStdString());

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

  SPDLOG_INFO("开始加载组件...");

  // 生命周期管理：
  // 1. 阶段1 (核心初始化): CoreComponent (依赖根节点) 首先初始化，设置管理器。
  // 2. 阶段2 (插件初始化): 其他插件初始化，注册上下文/提供者。
  // 3. 阶段3 (完成): 所有初始化完成后，InitialisationFinishedEvent 按相反顺序运行。
  //    CoreComponent 最后运行，构建 UI 并显示窗口。
  component_loader->LoadComponents([disabled_components](sss::extsystem::Component* component) -> bool {
    auto component_id = (component->Name() + "." + component->Vendor()).toLower();
    bool should_load = !disabled_components.contains(component_id);

    SPDLOG_INFO("组件 {}: 可被禁用={}, 应该加载={}", component_id.toStdString(),
                component->CanBeDisabled() ? "true" : "false", should_load ? "true" : "false");

    return should_load;
  });

  auto components = component_loader->Components();
  SPDLOG_INFO("总共加载的组件数: {}", components.size());

  SPDLOG_INFO("组件加载完成。");

  int exit_code;
  auto* main_window = (sss::extsystem::GetTObject<QMainWindow>());

  if (main_window != nullptr) {
    SPDLOG_INFO("找到主窗口: {}", (void*)main_window);

    SPDLOG_INFO("设置窗口图标 (Windows)");
    qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));

    SPDLOG_INFO("启动事件循环...");

    QTimer::singleShot(kSplashscreenTimeout, [=]() {
      SPDLOG_INFO("关闭启动画面...");
      splash_screen->deleteLater();
    });

    exit_code = QApplication::exec();

    SPDLOG_INFO("事件循环退出，代码: {}", exit_code);
  } else {
    SPDLOG_ERROR("错误：找不到主窗口！应用程序将退出。");
    exit_code = 1;
  }

  component_loader->UnloadComponents();

  SPDLOG_INFO("卸载组件...");

  delete component_loader;
  SPDLOG_INFO("组件加载器已删除。");

  delete application_instance;
  SPDLOG_INFO("应用程序实例已删除。");

  return exit_code;
}
