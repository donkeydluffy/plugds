#include "Core.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>

#include "MainWindow.h"
#include "StatusbarManager.h"
#include "extsystem/IComponentManager.h"

sss::dscore::Core::Core() {
  SPDLOG_INFO("[Core] Core constructor called");

  main_window_ = std::make_unique<sss::dscore::MainWindow>();
  SPDLOG_INFO("[Core] Created MainWindow: {}", (void*)main_window_.get());

  sss::extsystem::AddObject(main_window_.get());
  SPDLOG_INFO("[Core] Added MainWindow to object manager");

  statusbar_manager_ = std::make_unique<StatusbarManager>(main_window_->statusBar());
  sss::extsystem::AddObject(statusbar_manager_.get());
  SPDLOG_INFO("[Core] Created and registered StatusbarManager");

  random_generator_ = std::make_unique<std::mt19937>(random_device_());
  SPDLOG_INFO("[Core] Random generator initialized");
}

sss::dscore::Core::~Core() {
  SPDLOG_INFO("[Core] Core destructor called");

  if (statusbar_manager_) {
    sss::extsystem::RemoveObject(statusbar_manager_.get());
    SPDLOG_INFO("[Core] Removed StatusbarManager from object manager");
  }

  if (main_window_) {
    sss::extsystem::RemoveObject(main_window_.get());
    SPDLOG_INFO("[Core] Removed MainWindow from object manager");
  }
  // unique_ptr auto deletes main_window_

  // unique_ptr auto deletes random_generator_
}

auto sss::dscore::Core::Open() -> void {
  SPDLOG_INFO("[Core] Open() called, emitting CoreOpened signal");
  Q_EMIT CoreOpened();
}

auto sss::dscore::Core::GetMainWindow() -> QMainWindow* { return main_window_.get(); }

auto sss::dscore::Core::Random(int minimum_value, int maximum_value) -> int {
  std::uniform_int_distribution<uint16_t> dist(minimum_value, maximum_value);

  return dist(*random_generator_);
}

auto sss::dscore::Core::StorageFolder() -> QString {
  auto application_dir = QDir(qApp->applicationDirPath());

  if (QDir(application_dir).exists("data")) {
    return QDir::cleanPath(application_dir.absolutePath() + "/data");
  }

  return QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(0) + "/" + qApp->organizationName() +
         "/" + qApp->applicationName();
}

auto sss::dscore::Core::ApplicationContextMenu() -> IActionContainer* { return main_window_->ApplicationContextMenu(); }
