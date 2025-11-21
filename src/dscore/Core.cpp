#include "Core.h"

#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>

#include "MainWindow.h"

#include <spdlog/spdlog.h>

sss::dscore::Core::Core() : main_window_(new sss::dscore::MainWindow) {
  SPDLOG_INFO("[Core] Core constructor called");
  SPDLOG_INFO("[Core] Created MainWindow: {}", (void*)main_window_);

  sss::extsystem::AddObject(main_window_);
  SPDLOG_INFO("[Core] Added MainWindow to object manager");

  random_generator_ = new std::mt19937(random_device_());
  SPDLOG_INFO("[Core] Random generator initialized");
}

sss::dscore::Core::~Core() {
  SPDLOG_INFO("[Core] Core destructor called");

  sss::extsystem::RemoveObject(main_window_);
  SPDLOG_INFO("[Core] Removed MainWindow from object manager");

  delete random_generator_;
  SPDLOG_INFO("[Core] Random generator deleted");

  delete main_window_;
  SPDLOG_INFO("[Core] MainWindow deleted");
}

auto sss::dscore::Core::Open() -> void {
  SPDLOG_INFO("[Core] Open() called, emitting CoreOpened signal");
  Q_EMIT CoreOpened();
}

auto sss::dscore::Core::GetMainWindow() -> QMainWindow* { return main_window_; }

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

auto sss::dscore::Core::ApplicationContextMenu() -> IMenu* { return main_window_->ApplicationContextMenu(); }
