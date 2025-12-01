#include "MainWindow.h"

#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QEvent>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
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

  // Configure TabBar properties for better text display
  QTabBar* tab_bar = main_tab_widget_->tabBar();
  if (tab_bar != nullptr) {
    // Allow tabs to size based on content
    tab_bar->setExpanding(false);  // Don't automatically expand tabs
    // Text elide mode: ellipsis at the end if too long
    tab_bar->setElideMode(Qt::TextElideMode::ElideRight);
  }

  main_tab_widget_->setVisible(false);
  ui_->splitter->addWidget(main_tab_widget_);

  qApp->setWindowIcon(QIcon(":/app/AppIcon.ico"));

  showMaximized();

  setWindowTitle(QString(tr("Ds")));
}

sss::dscore::MainWindow::~MainWindow() { delete ui_; }

auto sss::dscore::MainWindow::updateTitlebar() -> void {}

auto sss::dscore::MainWindow::TabWidget() const -> QTabWidget* { return main_tab_widget_; }

#include "dscore/IActionContainer.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"

// ... existing includes ...

auto sss::dscore::MainWindow::Initialise() -> void {
  createDefaultCommands();
  registerDefaultCommands();
  setupToolbarSizeMenu();

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &MainWindow::updateIcons);
    // Apply initial icons based on current theme (assuming default or already loaded)
    // We can't easily get current theme ID from interface if not exposed,
    // but ThemeService has Theme() object which has ID.
    if (theme_service->Theme() != nullptr) {
      updateIcons(theme_service->Theme()->Id());
    } else {
      updateIcons("light");  // Default fallback
    }
  }
}

auto sss::dscore::MainWindow::setupToolbarSizeMenu() -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager == nullptr) return;

  // Create "View" menu if not exists (usually it does or we create it)
  // We check if we have a View menu in constants, if not we can put it in View or Settings
  // For now, let's put it in a new "View" menu or append to "Application" menu (which is usually main menu bar)

  // Ensure View menu exists
  auto* view_menu = command_manager->CreateActionContainer(
      "Ds.Menu.View", sss::dscore::ContainerType::kMenu,
      command_manager->FindContainer(sss::dscore::constants::menubars::kApplication), 300);
  // Add title "View"
  // Note: IActionContainer doesn't easily support setting title unless it was created with title.
  // But here we are creating a container. The ID "Ds.Menu.View" is internal.
  // We need to check if we can set the title.
  // The current implementation of CreateActionContainer for Menu creates QMenu.
  // We need to access the QMenu to set title.
  if (view_menu->GetWidget() != nullptr) {
    qobject_cast<QMenu*>(view_menu->GetWidget())->setTitle(tr("View"));
  }

  view_menu->InsertGroup("Ds.Group.View.Toolbar", 100);

  auto* size_menu_container = command_manager->CreateActionContainer("Ds.Menu.View.ToolbarSize",
                                                                     sss::dscore::ContainerType::kMenu, view_menu, 100);
  if (size_menu_container->GetWidget() != nullptr) {
    qobject_cast<QMenu*>(size_menu_container->GetWidget())->setTitle(tr("Toolbar Icon Size"));
  }
  size_menu_container->InsertGroup("Ds.Group.ToolbarSize", 0);

  auto add_size_action = [&](const QString& name, int size) {
    auto* action = new QAction(name, this);
    action->setCheckable(true);
    connect(action, &QAction::triggered, [this, size, action, command_manager]() {
      // Update toolbar size
      auto* tb_container = command_manager->FindContainer(sss::dscore::constants::toolbars::kMainToolbar);
      if (tb_container && tb_container->GetWidget()) {
        if (auto* tb = qobject_cast<QToolBar*>(tb_container->GetWidget())) {
          tb->setIconSize(QSize(size, size));
        }
      }
      // Update check state
      // Ideally we would have a QActionGroup, but for brevity:
      // Uncheck others? Handled by user manually for now or we need a group.
      // Let's just set size.
    });
    return action;
  };

  auto* group = new QActionGroup(this);

  auto* small = add_size_action(tr("Small (16px)"), 16);
  auto* medium = add_size_action(tr("Medium (24px)"), 24);
  auto* large = add_size_action(tr("Large (32px)"), 32);

  group->addAction(small);
  group->addAction(medium);
  group->addAction(large);
  medium->setChecked(true);  // Default

  // Register actions wrapping them in commands? Or just append QActions to menu container?
  // ActionContainer::AppendCommand takes ICommand* or command ID.
  // We can't easily append raw QActions via ActionContainer interface unless we register them as commands.
  // Let's register them.

  auto register_and_add = [&](QAction* action, const QString& id) {
    command_manager->RegisterAction(action, id, sss::dscore::kGlobalContext);
    size_menu_container->AppendCommand(id, "Ds.Group.ToolbarSize");
  };

  register_and_add(small, "Ds.Command.View.SizeSmall");
  register_and_add(medium, "Ds.Command.View.SizeMedium");
  register_and_add(large, "Ds.Command.View.SizeLarge");
}

auto sss::dscore::MainWindow::updateIcons(const QString& /*theme_id*/) -> void {  // NOLINT
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();

  if ((command_manager == nullptr) || (theme_service == nullptr)) return;

  const QString base_path = ":/dscore/resources/icons";

  auto set_icon = [&](const QString& cmd_id, const QString& filename) {
    auto* cmd = command_manager->FindCommand(cmd_id);
    if (cmd && cmd->Action()) {
      cmd->Action()->setIcon(theme_service->GetIcon(base_path, filename));
    }
  };

  set_icon(sss::dscore::constants::commands::kNew, "file_new.svg");
  set_icon(sss::dscore::constants::commands::kOpen, "file_open.svg");
  set_icon(sss::dscore::constants::commands::kSave, "file_save.svg");
  set_icon(sss::dscore::constants::commands::kPrint, "file_print.svg");

  set_icon(sss::dscore::constants::commands::kPreferences, "app_preferences.svg");
  set_icon(sss::dscore::constants::commands::kQuit, "app_quit.svg");
  set_icon(sss::dscore::constants::commands::kAbout, "help_about.svg");
  set_icon(sss::dscore::constants::commands::kAboutComponents, "help_about.svg");

  set_icon(sss::dscore::constants::commands::kCut, "edit_cut.svg");
  set_icon(sss::dscore::constants::commands::kCopy, "edit_copy.svg");
  set_icon(sss::dscore::constants::commands::kPaste, "edit_paste.svg");
}

auto sss::dscore::MainWindow::createDefaultCommands() -> void {
  // create the commands, these are essentially placeholders.  Commands can be added to menus, buttons,
  // shortcut keys etc.

  createCommand(sss::dscore::constants::commands::kOpen, nullptr);
  createCommand(sss::dscore::constants::commands::kNew, nullptr);
  createCommand(sss::dscore::constants::commands::kSave, nullptr);
  createCommand(sss::dscore::constants::commands::kPrint, nullptr);

  createCommand(sss::dscore::constants::commands::kAbout, nullptr, QAction::ApplicationSpecificRole);
  createCommand(sss::dscore::constants::commands::kAboutComponents, nullptr, QAction::ApplicationSpecificRole);
  createCommand(sss::dscore::constants::commands::kPreferences, nullptr, QAction::PreferencesRole);
  createCommand(sss::dscore::constants::commands::kQuit, nullptr, QAction::QuitRole);

  createCommand(sss::dscore::constants::commands::kCut, nullptr);
  createCommand(sss::dscore::constants::commands::kCopy, nullptr);
  createCommand(sss::dscore::constants::commands::kPaste, nullptr);

  // create the menus, we create a main menu bar, then sub menus on that (File, Edit, Help etc).  In each
  // menu we then create groups, this allows us to reserve sections of the menu for specific items, components
  // can use these groups to add their commands at specific locations in a menu.

  createMenu(sss::dscore::constants::menubars::kApplication);

  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager != nullptr) {
    auto* main_toolbar = command_manager->CreateActionContainer(sss::dscore::constants::toolbars::kMainToolbar,
                                                                sss::dscore::ContainerType::kToolBar, nullptr, 100);

    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupFileNew,
                              sss::dscore::constants::menugroups::kWeightNew);
    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupFileOpen,
                              sss::dscore::constants::menugroups::kWeightOpen);
    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupFileSave,
                              sss::dscore::constants::menugroups::kWeightSave);

    main_toolbar->AppendCommand(sss::dscore::constants::commands::kNew,
                                sss::dscore::constants::menugroups::kGroupFileNew);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kOpen,
                                sss::dscore::constants::menugroups::kGroupFileOpen);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kSave,
                                sss::dscore::constants::menugroups::kGroupFileSave);
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
  addMenuCommand(sss::dscore::constants::commands::kNew, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFileNew);
  addMenuCommand(sss::dscore::constants::commands::kOpen, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFileOpen);
  addMenuCommand(sss::dscore::constants::commands::kSave, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFileSave);
  addMenuCommand(sss::dscore::constants::commands::kPrint, sss::dscore::constants::menus::kFile,
                 sss::dscore::constants::menugroups::kGroupFilePrint);

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
