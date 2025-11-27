#include "MainWindow.h"

#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QEvent>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSystemTrayIcon>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QtGlobal>

#include "dscore/CoreConstants.h"
#include "dscore/ICommandManager.h"
#include "ui_MainWindow.h"

// #include <spdlog/spdlog.h>

sss::dscore::MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new sss::dscore::Ui::MainWindow), application_hidden_(false) {
  // spdlog::set_level(spdlog::level::trace);

  ui_->setupUi(this);

  main_tab_widget_ = new QTabWidget(this);
  main_tab_widget_->setObjectName("main_tab_widget");
  main_tab_widget_->setTabPosition(QTabWidget::East);
  main_tab_widget_->setVisible(false);
  ui_->splitter->addWidget(main_tab_widget_);

  qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));

  showMaximized();

  setWindowTitle(QString(tr("Ds")));
}

sss::dscore::MainWindow::~MainWindow() { delete ui_; }

auto sss::dscore::MainWindow::updateTitlebar() -> void {}

auto sss::dscore::MainWindow::TabWidget() const -> QTabWidget* { return main_tab_widget_; }

auto sss::dscore::MainWindow::Initialise() -> void {
  createDefaultCommands();
  registerDefaultCommands();
}

auto sss::dscore::MainWindow::createDefaultCommands() -> void {
  // create the commands, these are essentially placeholders.  Commands can be added to menus, buttons,
  // shortcut keys etc.

  createCommand(sss::dscore::constants::commands::kOpen, nullptr);
  createCommand(sss::dscore::constants::commands::kAbout, nullptr, QAction::ApplicationSpecificRole);
  createCommand(sss::dscore::constants::commands::kAboutComponents, nullptr, QAction::ApplicationSpecificRole);
  createCommand(sss::dscore::constants::commands::kPreferences, nullptr, QAction::PreferencesRole);
  createCommand(sss::dscore::constants::commands::kQuit, nullptr, QAction::QuitRole);

  // create the menus, we create a main menu bar, then sub menus on that (File, Edit, Help etc).  In each
  // menu we then create groups, this allows us to reserve sections of the menu for specific items, components
  // can use these groups to add their commands at specific locations in a menu.

  createMenu(sss::dscore::constants::menubars::kApplication);

  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager != nullptr) {
    auto* main_toolbar = command_manager->CreateActionContainer(sss::dscore::constants::toolbars::kMainToolbar,
                                                                sss::dscore::ContainerType::kToolBar, nullptr, 100);
    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupFileOpen,
                              sss::dscore::constants::menugroups::kWeightOpen);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kOpen,
                                sss::dscore::constants::menugroups::kGroupFileOpen);
  }

  // Define Standard Menu Order
  auto* file_menu =
      createMenu(sss::dscore::constants::menus::kFile, sss::dscore::constants::menubars::kApplication, 100);

  // Setup File Menu Groups with Standard Weights
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupFileNew,
                         sss::dscore::constants::menugroups::kWeightNew);
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupFileOpen,
                         sss::dscore::constants::menugroups::kWeightOpen);
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupFileSave,
                         sss::dscore::constants::menugroups::kWeightSave);
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupFilePrint,
                         sss::dscore::constants::menugroups::kWeightView);
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupAppPrefs,
                         sss::dscore::constants::menugroups::kWeightSettings);  // Added missing group
  file_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupFileExit,
                         sss::dscore::constants::menugroups::kWeightExit);

  createMenu(sss::dscore::constants::menus::kEdit, sss::dscore::constants::menubars::kApplication, 200);
  createMenu(sss::dscore::constants::menus::kHelp, sss::dscore::constants::menubars::kApplication, 900);

  // Place commands in their correct groups
  addMenuCommand(sss::dscore::constants::commands::kOpen, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFileOpen);

  addMenuCommand(sss::dscore::constants::commands::kPreferences, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupAppPrefs);

  addMenuCommand(sss::dscore::constants::commands::kQuit, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFileExit);

  addMenuCommand(sss::dscore::constants::commands::kAbout, sss::dscore::constants::menus::kHelp);
  addMenuCommand(sss::dscore::constants::commands::kAboutComponents, sss::dscore::constants::menus::kHelp);

  addMenuCommand(sss::dscore::constants::commands::kCut, sss::dscore::constants::menus::kEdit);
  addMenuCommand(sss::dscore::constants::commands::kCopy, sss::dscore::constants::menus::kEdit);
  addMenuCommand(sss::dscore::constants::commands::kPaste, sss::dscore::constants::menus::kEdit);

  if (sss::dscore::IContextManager::GetInstance() != nullptr) {
    sss::dscore::IContextManager::GetInstance()->SetContext(sss::dscore::kGlobalContext);
  }
}

auto sss::dscore::MainWindow::registerDefaultCommands() -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  auto* about_components_action =
      new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kAboutComponents));

  about_components_action->setEnabled(true);
  about_components_action->setMenuRole(QAction::ApplicationSpecificRole);

  command_manager->RegisterAction(about_components_action, sss::dscore::constants::commands::kAboutComponents,
                                  sss::dscore::kGlobalContext);

  preferences_action_ =
      new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kPreferences));

  preferences_action_->setEnabled(true);
  preferences_action_->setMenuRole(QAction::PreferencesRole);

  command_manager->RegisterAction(preferences_action_, sss::dscore::constants::commands::kPreferences,
                                  sss::dscore::kGlobalContext);

  quit_action_ = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kQuit));

  quit_action_->setEnabled(true);
  quit_action_->setMenuRole(QAction::QuitRole);

  command_manager->RegisterAction(quit_action_, sss::dscore::constants::commands::kQuit, sss::dscore::kGlobalContext);

  connect(quit_action_, &QAction::triggered, [this](bool) { QGuiApplication::quit(); });

  about_action_ = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kAbout));

  about_action_->setEnabled(true);
  about_action_->setMenuRole(QAction::ApplicationSpecificRole);

  command_manager->RegisterAction(about_action_, sss::dscore::constants::commands::kAbout, sss::dscore::kGlobalContext);

  show_application_ =
      new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kShowApplication));

  show_application_->setEnabled(true);
  show_application_->setMenuRole(QAction::ApplicationSpecificRole);

  command_manager->RegisterAction(show_application_, sss::dscore::constants::commands::kShowApplication,
                                  sss::dscore::kGlobalContext);

  hide_application_ =
      new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kHideApplication));

  hide_application_->setEnabled(true);
  hide_application_->setMenuRole(QAction::ApplicationSpecificRole);

  command_manager->RegisterAction(hide_application_, sss::dscore::constants::commands::kHideApplication,
                                  sss::dscore::kGlobalContext);
}

auto sss::dscore::MainWindow::createCommand(QString command_id, QAbstractButton* button,  // NOLINT
                                            QAction::MenuRole menu_role) -> sss::dscore::ICommand* {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  if (command_manager == nullptr) {
    return nullptr;
  }

  auto* action = new QAction(sss::dscore::constants::CommandText(command_id));

  action->setMenuRole(menu_role);

  auto* command = command_manager->RegisterAction(action, command_id, sss::dscore::kGlobalContext);

  if (button != nullptr) {
    command->AttachToWidget(button);
  }

  action->setEnabled(false);

  return command;
}

auto sss::dscore::MainWindow::createMenu(const QString& menu_id, const QString& parent_menu_id, int order)  // NOLINT
    -> sss::dscore::IActionContainer* {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  if (command_manager == nullptr) {
    return nullptr;
  }

  sss::dscore::IActionContainer* parent_menu = nullptr;

  if (!parent_menu_id.isNull()) {
    parent_menu = command_manager->FindContainer(parent_menu_id);
  }

  return command_manager->CreateActionContainer(menu_id, sss::dscore::ContainerType::kMenu, parent_menu, order);
}

auto sss::dscore::MainWindow::findMenu(const QString& menu_id) -> sss::dscore::IActionContainer* {  // NOLINT
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  if (command_manager == nullptr) {
    return nullptr;
  }

  return command_manager->FindContainer(menu_id);
}

auto sss::dscore::MainWindow::addMenuCommand(const QString& command_id, const QString& menu_id,  // NOLINT
                                             QString group_id) -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  if (command_manager == nullptr) {
    return;
  }

  auto* menu = command_manager->FindContainer(menu_id);

  if (menu == nullptr) {
    return;
  }

  auto* command = command_manager->FindCommand(command_id);

  if (group_id.isNull()) {
    group_id = sss::dscore::constants::menugroups::kGroupFileNew;  // Default fallback
  }

  menu->AppendCommand(command, group_id);
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

auto sss::dscore::MainWindow::ApplicationContextMenu() -> sss::dscore::IActionContainer* {  // NOLINT

  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  auto* context_menu = command_manager->CreateActionContainer(QString(), sss::dscore::ContainerType::kMenu, nullptr, 0);

  context_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupAppPrefs,
                            sss::dscore::constants::menugroups::kWeightSettings);

  context_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupAppExit,
                            sss::dscore::constants::menugroups::kWeightExit);

  context_menu->AppendCommand(sss::dscore::constants::commands::kAbout,
                              sss::dscore::constants::menugroups::kGroupAppPrefs);

  context_menu->AppendCommand(sss::dscore::constants::commands::kPreferences,
                              sss::dscore::constants::menugroups::kGroupAppPrefs);

  context_menu->AppendCommand(sss::dscore::constants::commands::kQuit,
                              sss::dscore::constants::menugroups::kGroupAppExit);

  return context_menu;
}
