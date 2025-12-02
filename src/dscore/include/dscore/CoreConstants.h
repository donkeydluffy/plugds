#pragma once

#include <QMap>
#include <QObject>
#include <QtGlobal>
#include <functional>

#include "dscore/CoreStrings.h"

namespace sss::dscore::constants {
namespace menubars {
constexpr auto kApplication = "Ds.Application";
};

namespace toolbars {
constexpr auto kMainToolbar = "Ds.MainToolbar";
};

namespace menus {
constexpr auto kSettings = "Ds.Settings";
constexpr auto kHelp = "Ds.Help";
constexpr auto kApplication = "Ds.Application";
// Submenus
constexpr auto kLanguage = "Ds.Menu.Language";
constexpr auto kTheme = "Ds.Menu.Theme";
}  // namespace menus

namespace menugroups {
// Standard Group Weights
constexpr int kWeightTop = 0;
constexpr int kWeightHigh = 100;
constexpr int kWeightNormal = 500;
constexpr int kWeightLow = 900;

// Settings Menu Groups
constexpr auto kGroupLanguage = "Ds.Group.Language";
constexpr auto kGroupTheme = "Ds.Group.Theme";

// Help Menu Groups
constexpr auto kGroupHelp = "Ds.Group.Help";

// Toolbar Groups
constexpr auto kGroupToolbarMain = "Ds.Group.Toolbar.Main";
}  // namespace menugroups

namespace commands {
constexpr auto kOpen = "Ds.Open";
constexpr auto kSave = "Ds.Save";

constexpr auto kAbout = "Ds.About";

// Language
constexpr auto kLangEnglish = "Ds.Lang.English";
constexpr auto kLangChinese = "Ds.Lang.Chinese";

// Theme
constexpr auto kThemeDark = "Ds.Theme.Dark";
constexpr auto kThemeLight = "Ds.Theme.Light";

const QMap<QString, QString> kMap = {{kOpen, QT_TR_NOOP("Open")},
                                     {kSave, QT_TR_NOOP("Save")},
                                     {kAbout, QT_TR_NOOP("About DefinSight")},
                                     {kLangEnglish, QT_TR_NOOP("English")},
                                     {kLangChinese, QT_TR_NOOP("Chinese")},
                                     {kThemeDark, QT_TR_NOOP("Dark")},
                                     {kThemeLight, QT_TR_NOOP("Light")}};
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
      {commands::kAbout, &CoreStrings::About},
      {commands::kOpen, &CoreStrings::Open},
      {commands::kSave, &CoreStrings::Save},
  };

  if (kCoreCommandMap.contains(string)) {
    return kCoreCommandMap[string]();
  }

  if (commands::kMap.contains(string)) return QObject::tr(commands::kMap[string].toUtf8());

  return string;
}
}  // namespace sss::dscore::constants
