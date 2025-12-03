#include "CoreUIProvider.h"

#include <QAction>
#include <QObject>

#include "dscore/CoreConstants.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommand.h"
#include "dscore/ICommandManager.h"
#include "dscore/ILanguageService.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

CoreUIProvider::CoreUIProvider(QObject* parent) : QObject(parent) {}

CoreUIProvider::~CoreUIProvider() = default;

void CoreUIProvider::RegisterCommands(sss::dscore::ICommandManager* command_manager) {
  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();

  // --- About ---
  auto* about_action = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kAbout));
  about_action->setMenuRole(QAction::ApplicationSpecificRole);
  command_manager->RegisterAction(about_action, sss::dscore::constants::commands::kAbout, sss::dscore::kGlobalContext);

  // --- Open ---
  auto* open_action = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kOpen));
  command_manager->RegisterAction(open_action, sss::dscore::constants::commands::kOpen, sss::dscore::kGlobalContext);

  // --- Save ---
  auto* save_action = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kSave));
  command_manager->RegisterAction(save_action, sss::dscore::constants::commands::kSave, sss::dscore::kGlobalContext);

  // --- Language: English ---
  auto* act_en = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kLangEnglish));
  connect(act_en, &QAction::triggered, [lang_service]() {
    if (lang_service) lang_service->SwitchLanguage(QLocale("en_US"));
  });
  command_manager->RegisterAction(act_en, sss::dscore::constants::commands::kLangEnglish, sss::dscore::kGlobalContext);

  // --- Language: Chinese ---
  auto* act_zh = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kLangChinese));
  connect(act_zh, &QAction::triggered, [lang_service]() {
    if (lang_service) lang_service->SwitchLanguage(QLocale("zh_CN"));
  });
  command_manager->RegisterAction(act_zh, sss::dscore::constants::commands::kLangChinese, sss::dscore::kGlobalContext);

  // --- Theme: Dark ---
  auto* act_dark = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kThemeDark));
  connect(act_dark, &QAction::triggered, [theme_service]() {
    if (theme_service) theme_service->LoadTheme("dark");
  });
  command_manager->RegisterAction(act_dark, sss::dscore::constants::commands::kThemeDark, sss::dscore::kGlobalContext);

  // --- Theme: Light ---
  auto* act_light = new QAction(sss::dscore::constants::CommandText(sss::dscore::constants::commands::kThemeLight));
  connect(act_light, &QAction::triggered, [theme_service]() {
    if (theme_service) theme_service->LoadTheme("light");
  });
  command_manager->RegisterAction(act_light, sss::dscore::constants::commands::kThemeLight,
                                  sss::dscore::kGlobalContext);

  // --- Icon Updates ---
  auto update_icons = [command_manager, theme_service](const QString& /*theme_id*/) {
    if (!command_manager || !theme_service) return;

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
  };

  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, update_icons);
    // Initial update
    if (theme_service->Theme() != nullptr) {
      update_icons(theme_service->Theme()->Id());
    } else {
      update_icons("light");
    }
  }
}

void CoreUIProvider::ContributeToMenu(sss::dscore::ICommandManager* command_manager) {
  // Find the Application MenuBar (created by MenuAndToolbarManager)
  auto* app_menu_bar = command_manager->FindContainer(sss::dscore::constants::menubars::kApplication);

  // Settings Menu
  // Note: passing app_menu_bar as parent ensures it is attached to the menu bar, not creating a wrapper.
  auto* settings_menu = command_manager->CreateActionContainer(sss::dscore::constants::menus::kSettings,
                                                               sss::dscore::ContainerType::kMenu, app_menu_bar, 100);

  if (settings_menu != nullptr) {
    settings_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupLanguage, 100);
    settings_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupTheme, 200);
  }

  // Language Submenu
  auto* lang_menu = command_manager->CreateActionContainer(sss::dscore::constants::menus::kLanguage,
                                                           sss::dscore::ContainerType::kMenu, settings_menu, 100);
  if (lang_menu != nullptr) {
    lang_menu->InsertGroup("Ds.Group.Default", 0);
    lang_menu->AppendCommand(sss::dscore::constants::commands::kLangEnglish, "Ds.Group.Default");
    lang_menu->AppendCommand(sss::dscore::constants::commands::kLangChinese, "Ds.Group.Default");
  }

  // Theme Submenu
  auto* theme_menu = command_manager->CreateActionContainer(sss::dscore::constants::menus::kTheme,
                                                            sss::dscore::ContainerType::kMenu, settings_menu, 200);
  if (theme_menu != nullptr) {
    theme_menu->InsertGroup("Ds.Group.Default", 0);
    theme_menu->AppendCommand(sss::dscore::constants::commands::kThemeDark, "Ds.Group.Default");
    theme_menu->AppendCommand(sss::dscore::constants::commands::kThemeLight, "Ds.Group.Default");
  }

  // Help Menu
  auto* help_menu = command_manager->CreateActionContainer(sss::dscore::constants::menus::kHelp,
                                                           sss::dscore::ContainerType::kMenu, app_menu_bar, 900);
  if (help_menu != nullptr) {
    help_menu->InsertGroup(sss::dscore::constants::menugroups::kGroupHelp, 100);
    help_menu->AppendCommand(sss::dscore::constants::commands::kAbout, sss::dscore::constants::menugroups::kGroupHelp);
  }
}

void CoreUIProvider::ContributeToToolbar(sss::dscore::ICommandManager* command_manager) {
  auto* main_toolbar = command_manager->CreateActionContainer(sss::dscore::constants::toolbars::kMainToolbar,
                                                              sss::dscore::ContainerType::kToolBar, nullptr, 100);

  if (main_toolbar != nullptr) {
    main_toolbar->InsertGroup(sss::dscore::constants::menugroups::kGroupToolbarMain, 100);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kOpen,
                                sss::dscore::constants::menugroups::kGroupToolbarMain);
    main_toolbar->AppendCommand(sss::dscore::constants::commands::kSave,
                                sss::dscore::constants::menugroups::kGroupToolbarMain);
  }
}

}  // namespace sss::dscore
