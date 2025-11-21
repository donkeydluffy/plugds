/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 27/03/2020.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

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
  // 初始化spdlog
  auto file_logger = spdlog::basic_logger_mt("main", "pingnoo.log", true);
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::info);

  SPDLOG_INFO("Application starting...");

  qApp->setApplicationName("Pingnoo");
  qApp->setOrganizationName("Nedrysoft");

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

  SPDLOG_INFO("Created splash screen.");

  splash_screen->show();

  auto* component_manager = sss::extsystem::IComponentManager::GetInstance();

  SPDLOG_INFO("Component manager instance obtained.");

  component_manager->AddObject(component_loader);

  // SPDLOG_DEBUG("Application started.");

  QStringList component_locations = QStringList() << "APPDIR" << "PINGNOO_COMPONENT_DIR";

  SPDLOG_INFO("Component locations: {}", component_locations.join(", ").toStdString());

  for (const auto& dir_name : component_locations) {
    if (QProcessEnvironment::systemEnvironment().contains(dir_name)) {
      auto dir_path = QProcessEnvironment::systemEnvironment().value(dir_name);
      auto components_path = dir_path + "/Components";

      SPDLOG_INFO("Found component path via {}: {}", dir_name.toStdString(), components_path.toStdString());

      component_loader->AddComponents(components_path);
    }
  }

  if (application_dir.exists("Components")) {
    auto components_path = application_dir.absoluteFilePath("Components");

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

  SPDLOG_INFO("Starting to load components...");

  component_loader->LoadComponents([disabled_components](sss::extsystem::Component* component) -> bool {
    auto component_id = (component->Name() + "." + component->Vendor()).toLower();
    bool should_load = !disabled_components.contains(component_id);

    SPDLOG_INFO("Component {}: CanBeDisabled={}, ShouldLoad={}", component_id.toStdString(),
                 component->CanBeDisabled() ? "true" : "false", should_load ? "true" : "false");

    return should_load;
  });

  auto components = component_loader->Components();
  SPDLOG_INFO("Total components loaded: {}", components.size());

  SPDLOG_INFO("Component loading completed.");

  int exit_code;
  auto main_window = (sss::extsystem::GetObject<QMainWindow>());

  if (main_window != nullptr) {
    SPDLOG_INFO("Found main window: {}", (void*)main_window);

#if defined(Q_OS_WINDOWS)
    SPDLOG_INFO("Setting window icon (Windows)");
    qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));
#else
    SPDLOG_INFO("Setting window icon (Other)");
    qApp->setWindowIcon(QIcon(":/app/images/appicon/colour/appicon-512x512@2x.png"));
#endif

    SPDLOG_INFO("Starting event loop...");

    QTimer::singleShot(kSplashscreenTimeout, [=]() {
      SPDLOG_INFO("Closing splash screen...");
      splash_screen->deleteLater();
    });

    exit_code = QApplication::exec();

    SPDLOG_INFO("Event loop exited with code: {}", exit_code);
  } else {
    SPDLOG_ERROR("ERROR: Main window not found! Application will exit.");
    exit_code = 1;
  }

  component_loader->UnloadComponents();

  SPDLOG_INFO("Unloading components...");

  delete component_loader;
  SPDLOG_INFO("Component loader deleted.");

  delete application_instance;
  SPDLOG_INFO("Application instance deleted.");

  return exit_code;
}
