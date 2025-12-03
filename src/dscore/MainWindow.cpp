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
#include "dscore/ICommandManager.h"
#include "dscore/ICommandProvider.h"
#include "dscore/IContextManager.h"
#include "dscore/IMenuProvider.h"
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

auto sss::dscore::MainWindow::Initialise() -> void {  // NOLINT
  if (sss::dscore::IContextManager::GetInstance() != nullptr) {
    sss::dscore::IContextManager::GetInstance()->SetContext(sss::dscore::kGlobalContext);
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
