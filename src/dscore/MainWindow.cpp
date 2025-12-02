#include "MainWindow.h"

#include <spdlog/spdlog.h>  // Required for SPDLOG_INFO

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

#include "ModeManager.h"
#include "WorkbenchLayout.h"
#include "dscore/CoreConstants.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/ILanguageService.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"
#include "ui_MainWindow.h"

sss::dscore::MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new sss::dscore::Ui::MainWindow),
      application_hidden_(false),
      mode_manager_(new sss::dscore::ModeManager(this)),
      workbench_layout_(new sss::dscore::WorkbenchLayout(this)) {  // Create Global Workbench
  // spdlog::set_level(spdlog::level::trace);

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
  createDefaultCommands();
  registerDefaultCommands();

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &MainWindow::updateIcons);
    if (theme_service->Theme() != nullptr) {
      updateIcons(theme_service->Theme()->Id());
    } else {
      updateIcons("light");  // Default fallback
    }
  }
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

  set_icon(sss::dscore::constants::commands::kOpen, "file_open.svg");
  set_icon(sss::dscore::constants::commands::kSave, "file_save.svg");
  set_icon(sss::dscore::constants::commands::kAbout, "help_about.svg");

  // Add icons for new commands if available, or reuse existing ones
  // Assuming we have these icons or reused them.
  // Based on file list, we might need to check what icons we have.
  // But per requirement "cleanup resources", we should eventually delete unused ones.
  // For now let's assume standard names or placeholders.
}

auto sss::dscore::MainWindow::createDefaultCommands() -> void {
  createCommand(sss::dscore::constants::commands::kOpen, nullptr);
  createCommand(sss::dscore::constants::commands::kSave, nullptr);
  createCommand(sss::dscore::constants::commands::kAbout, nullptr, QAction::ApplicationSpecificRole);

  // Create menus
  createMenu(sss::dscore::constants::menubars::kApplication);

  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager != nullptr) {
    // 1. Toolbar: Open, Save
    auto* main_toolbar = command_manager->CreateActionContainer(sss::dscore::constants::toolbars::kMainToolbar,
                                                                sss::dscore::ContainerType::kToolBar, nullptr, 100);

    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupToolbarMain, 100);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kOpen,
                                sss::dscore::constants::menugroups::kGroupToolbarMain);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kSave,
                                sss::dscore::constants::menugroups::kGroupToolbarMain);
  }

  // 2. Menu: Settings (Priority > Help)
  // Priority: Settings (100), Help (900)
  // ws1 will inject at ~500

  auto* settings_menu =
      createMenu(sss::dscore::constants::menus::kSettings, sss::dscore::constants::menubars::kApplication, 100);

  // Language Submenu
  settings_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupLanguage, 100);
  auto* lang_menu = createMenu(sss::dscore::constants::menus::kLanguage, sss::dscore::constants::menus::kSettings, 100);
  if (lang_menu != nullptr) lang_menu->InsertGroup("Ds.Group.Default", 0);

  // Theme Submenu
  settings_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupTheme, 200);
  auto* theme_menu = createMenu(sss::dscore::constants::menus::kTheme, sss::dscore::constants::menus::kSettings, 200);
  if (theme_menu != nullptr) theme_menu->InsertGroup("Ds.Group.Default", 0);

  // 3. Menu: Help
  auto* help_menu =
      createMenu(sss::dscore::constants::menus::kHelp, sss::dscore::constants::menubars::kApplication, 900);

  help_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupHelp, 100);
  addMenuCommand(sss::dscore::constants::commands::kAbout, sss::dscore::constants::menus::kHelp,
                 sss::dscore::constants::menugroups::kGroupHelp);

  if (sss::dscore::IContextManager::GetInstance() != nullptr) {
    sss::dscore::IContextManager::GetInstance()->SetContext(sss::dscore::kGlobalContext);
  }
}

auto sss::dscore::MainWindow::registerDefaultCommands() -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();

  // --- About ---
  about_action_ = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kAbout));
  about_action_->setEnabled(true);
  about_action_->setMenuRole(QAction::ApplicationSpecificRole);
  command_manager->RegisterAction(about_action_, sss::dscore::constants::commands::kAbout, sss::dscore::kGlobalContext);

  // --- Language: English ---
  auto* act_en = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kLangEnglish));
  connect(act_en, &QAction::triggered, [lang_service]() {
    if (lang_service) lang_service->SwitchLanguage(QLocale("en_US"));
  });
  command_manager->RegisterAction(act_en, sss::dscore::constants::commands::kLangEnglish, sss::dscore::kGlobalContext);
  addMenuCommand(sss::dscore::constants::commands::kLangEnglish, sss::dscore::constants::menus::kLanguage);

  // --- Language: Chinese ---
  auto* act_zh = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kLangChinese));
  connect(act_zh, &QAction::triggered, [lang_service]() {
    if (lang_service) lang_service->SwitchLanguage(QLocale("zh_CN"));
  });
  command_manager->RegisterAction(act_zh, sss::dscore::constants::commands::kLangChinese, sss::dscore::kGlobalContext);
  addMenuCommand(sss::dscore::constants::commands::kLangChinese, sss::dscore::constants::menus::kLanguage);

  // --- Theme: Dark ---
  auto* act_dark = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kThemeDark));
  connect(act_dark, &QAction::triggered, [theme_service]() {
    if (theme_service) theme_service->LoadTheme("dark");
  });
  command_manager->RegisterAction(act_dark, sss::dscore::constants::commands::kThemeDark, sss::dscore::kGlobalContext);
  addMenuCommand(sss::dscore::constants::commands::kThemeDark, sss::dscore::constants::menus::kTheme);

  // --- Theme: Light ---
  auto* act_light = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kThemeLight));
  connect(act_light, &QAction::triggered, [theme_service]() {
    if (theme_service) theme_service->LoadTheme("light");
  });
  command_manager->RegisterAction(act_light, sss::dscore::constants::commands::kThemeLight,
                                  sss::dscore::kGlobalContext);
  addMenuCommand(sss::dscore::constants::commands::kThemeLight, sss::dscore::constants::menus::kTheme);

  // Note: Open and Save are registered in createCommand called by createDefaultCommands, but their actions are dummies.
  // Typically we would connect them to something. For now they are just buttons.
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

  // Enable Open/Save for demo purposes
  action->setEnabled(true);

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
                                             const QString& group_id) -> void {
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
    // If no group, append to end or default group?
    // ActionContainer::AppendCommand requires a group usually or finds one.
    // Let's try adding to a default group if we can, or just AppendCommand
    // (ActionContainer usually needs a group id)
    // Looking at previous code: group_id defaulted to FileNew.
    // We should use a generic group if null.
    menu->AppendCommand(command, "Ds.Group.Default");  // Fallback
  } else {
    menu->AppendCommand(command, group_id);
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
