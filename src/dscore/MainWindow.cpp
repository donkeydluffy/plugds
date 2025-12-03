#include "MainWindow.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

#include "ModeManager.h"
#include "WorkbenchLayout.h"
#include "dscore/CoreConstants.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/ICommandProvider.h"
#include "dscore/IContextManager.h"
#include "dscore/IMenuProvider.h"
#include "dscore/IStatusbarManager.h"
#include "dscore/IStatusbarProvider.h"
#include "dscore/IToolbarProvider.h"
#include "extsystem/IComponentManager.h"
#include "ui_MainWindow.h"

sss::dscore::MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new sss::dscore::Ui::MainWindow),
      application_hidden_(false),
      mode_manager_(new sss::dscore::ModeManager(this)),
      workbench_layout_(new sss::dscore::WorkbenchLayout(this)) {
  ui_->setupUi(this);

  // Set Global Workbench as Central Widget in Splitter
  ui_->splitter->addWidget(workbench_layout_);
  ui_->splitter->setStretchFactor(0, 1);  // Workbench takes full space

  qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));

  showMaximized();

  setWindowTitle(QString(tr("Ds")));

  // Initialize ModeManager and register services
  if (mode_manager_ != nullptr) {
    mode_manager_->SetGlobalWorkbench(workbench_layout_);
    sss::extsystem::AddObject(mode_manager_);  // Register IModeManager service

    // Register Global Workbench service
    sss::extsystem::AddObject(workbench_layout_);
  }
}

sss::dscore::MainWindow::~MainWindow() {
  if (mode_manager_ != nullptr) {
    sss::extsystem::RemoveObject(mode_manager_);
  }
  if (workbench_layout_ != nullptr) {
    sss::extsystem::RemoveObject(workbench_layout_);
  }
  delete ui_;
}

auto sss::dscore::MainWindow::updateTitlebar() -> void {}

auto sss::dscore::MainWindow::Initialise() -> void {
  loadUiExtensions();

  if (sss::dscore::IContextManager::GetInstance() != nullptr) {
    sss::dscore::IContextManager::GetInstance()->SetContext(sss::dscore::kGlobalContext);
  }
}

auto sss::dscore::MainWindow::loadUiExtensions() -> void {  // NOLINT
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("CommandManager is null in LoadUIExtensions");
    return;
  }

  // Initialize Root Containers (MenuBar)
  command_manager->CreateActionContainer(sss::dscore::constants::menubars::kApplication,
                                         sss::dscore::ContainerType::kMenuBar, nullptr, 0);

  // 1. Register Commands
  auto command_providers = sss::extsystem::GetTObjects<sss::dscore::ICommandProvider>();
  for (auto* provider : command_providers) {
    provider->RegisterCommands(command_manager);
  }

  // 2. Contribute Menus
  auto menu_providers = sss::extsystem::GetTObjects<sss::dscore::IMenuProvider>();
  for (auto* provider : menu_providers) {
    provider->ContributeToMenu(command_manager);
  }

  // 3. Contribute Toolbars
  auto toolbar_providers = sss::extsystem::GetTObjects<sss::dscore::IToolbarProvider>();
  for (auto* provider : toolbar_providers) {
    provider->ContributeToToolbar(command_manager);
  }

  // 4. Contribute Statusbar
  auto* statusbar_manager = sss::extsystem::GetTObject<sss::dscore::IStatusbarManager>();
  if (statusbar_manager != nullptr) {
    auto statusbar_providers = sss::extsystem::GetTObjects<sss::dscore::IStatusbarProvider>();
    for (auto* provider : statusbar_providers) {
      provider->ContributeToStatusbar(statusbar_manager);
    }
  }
}

void sss::dscore::MainWindow::closeEvent(QCloseEvent* close_event) { QMainWindow::closeEvent(close_event); }

void sss::dscore::MainWindow::changeEvent(QEvent* event) {
  if (event->type() == QEvent::LanguageChange) {
    auto* command_manager = sss::dscore::ICommandManager::GetInstance();
    if (command_manager != nullptr) {
      command_manager->RetranslateUi();
    }
    // Also retranslate MainWindow generic UI if any (setWindowTitle etc)
    setWindowTitle(QString(tr("Ds")));
  }
  QMainWindow::changeEvent(event);
}
