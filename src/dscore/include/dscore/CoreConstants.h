#pragma once

#include <QMap>
#include <QObject>
#include <QtGlobal>
#include <functional>

#include "dscore/CoreStrings.h"

namespace sss::dscore::constants {
// 主菜单栏
namespace menubars {
constexpr auto kMainMenubar = "Ds.MainMenubar";
};

// 主工具栏
namespace toolbars {
constexpr auto kMainToolbar = "Ds.MainToolbar";
};

// Core 菜单
namespace menus {
constexpr auto kSettings = "Ds.Settings";
constexpr auto kHelp = "Ds.Help";
// Settings 子菜单
constexpr auto kLanguage = "Ds.Settings.Language";
constexpr auto kTheme = "Ds.Settings.Theme";
}  // namespace menus

namespace menugroups {
// Settings Core Group
constexpr auto kSettingsCoreGroup = "Ds.Group.Settings.Core";
// Help Core Groups
constexpr auto kHelpCoreGroup = "Ds.Group.Help.Core";
// Toolbar Groups
constexpr auto kMainToolbarCore = "Ds.Group.MainToolbar.Core";
}  // namespace menugroups

namespace commands {
constexpr auto kOpen = "Ds.Open";
constexpr auto kSave = "Ds.Save";
constexpr auto kAbout = "Ds.About";
// Language
constexpr auto kLangEnglish = "Ds.Language.English";
constexpr auto kLangChinese = "Ds.Language.Chinese";
// Theme
constexpr auto kThemeDark = "Ds.Theme.Dark";
constexpr auto kThemeLight = "Ds.Theme.Light";
};  // namespace commands

inline QString MenuText(const QString& string) {
  static const QMap<QString, std::function<QString()>> kCoreMenuMap = {{menus::kSettings, &CoreStrings::Settings},
                                                                       {menus::kHelp, &CoreStrings::Help},
                                                                       {menus::kLanguage, &CoreStrings::Language},
                                                                       {menus::kTheme, &CoreStrings::Theme}};

  if (kCoreMenuMap.contains(string)) {
    return kCoreMenuMap[string]();
  }

  return string;
}

inline QString CommandText(const QString& string) {
  static const QMap<QString, std::function<QString()>> kCoreCommandMap = {
      {commands::kOpen, &CoreStrings::Open},
      {commands::kSave, &CoreStrings::Save},
      {commands::kAbout, &CoreStrings::About},
      {commands::kLangEnglish, &CoreStrings::EnglishLanguage},
      {commands::kLangChinese, &CoreStrings::ChineseLanguage},
      {commands::kThemeDark, &CoreStrings::DarkTheme},
      {commands::kThemeLight, &CoreStrings::LightTheme}};

  if (kCoreCommandMap.contains(string)) {
    return kCoreCommandMap[string]();
  }

  return string;
}
}  // namespace sss::dscore::constants
