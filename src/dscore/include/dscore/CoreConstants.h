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
constexpr auto kFile = "Ds.File";
constexpr auto kEdit = "Ds.Edit";
constexpr auto kHelp = "Ds.Help";
constexpr auto kApplication = "Ds.Application";

const QMap<QString, QString> kMap = {{kFile, QT_TR_NOOP("File")},
                                     {kEdit, QT_TR_NOOP("Edit")},
                                     {kHelp, QT_TR_NOOP("Help")},
                                     {kApplication, QT_TR_NOOP("Application")}};
}  // namespace menus

namespace menugroups {
// Standard Group Weights
constexpr int kWeightTop = 0;
constexpr int kWeightNew = 100;
constexpr int kWeightOpen = 200;
constexpr int kWeightSave = 300;
constexpr int kWeightEdit = 400;
constexpr int kWeightView = 500;
constexpr int kWeightTools = 600;
constexpr int kWeightSettings = 900;
constexpr int kWeightExit = 1000;

// File Menu Groups
constexpr auto kGroupFileNew = "Ds.Group.File.New";      // Weight 100
constexpr auto kGroupFileOpen = "Ds.Group.File.Open";    // Weight 200
constexpr auto kGroupFileSave = "Ds.Group.File.Save";    // Weight 300
constexpr auto kGroupFilePrint = "Ds.Group.File.Print";  // Weight 500
constexpr auto kGroupFileExit = "Ds.Group.File.Exit";    // Weight 1000

// Edit Menu Groups
constexpr auto kGroupEditUndo = "Ds.Group.Edit.Undo";  // Weight 100
constexpr auto kGroupEditClip = "Ds.Group.Edit.Clip";  // Weight 200 (Cut/Copy/Paste)

// App/Help Menu Groups
constexpr auto kGroupAppPrefs = "Ds.Group.App.Prefs";  // Weight 900
constexpr auto kGroupAppHelp = "Ds.Group.App.Help";    // Weight 100
constexpr auto kGroupAppExit = "Ds.Group.App.Exit";    // Weight 1000
}  // namespace menugroups

namespace commands {
constexpr auto kPreferences = "Ds.Preferences";

constexpr auto kOpen = "Ds.Open";
constexpr auto kQuit = "Ds.Quit";

constexpr auto kCut = "Ds.Cut";
constexpr auto kCopy = "Ds.Copy";
constexpr auto kPaste = "Ds.Paste";

constexpr auto kAbout = "Ds.About";

constexpr auto kAboutComponents = "Ds.AboutComponents";

constexpr auto kShowApplication = "Ds.ShowApplication";
constexpr auto kHideApplication = "Ds.HideApplication";

const QMap<QString, QString> kMap = {{kPreferences, QT_TR_NOOP("Preferences")},

                                     {kCut, QT_TR_NOOP("Cut")},
                                     {kCopy, QT_TR_NOOP("Copy")},
                                     {kPaste, QT_TR_NOOP("Paste")},

                                     {kOpen, QT_TR_NOOP("Open...")},
                                     {kQuit, QT_TR_NOOP("Exit")},

                                     {kAbout, QT_TR_NOOP("About DefinSight")},
                                     {kAboutComponents, QT_TR_NOOP("About Components")},

                                     {kShowApplication, QT_TR_NOOP("Show Application")},
                                     {kHideApplication, QT_TR_NOOP("Hide Application")}};
};  // namespace commands

inline QString MenuText(const QString& string) {
  static const QMap<QString, std::function<QString()>> kCoreMenuMap = {
      {menus::kFile, &CoreStrings::File}, {menus::kEdit, &CoreStrings::Edit}, {menus::kHelp, &CoreStrings::Help}};

  if (kCoreMenuMap.contains(string)) {
    return kCoreMenuMap[string]();
  }

  if (menus::kMap.contains(string)) return QObject::tr(menus::kMap[string].toUtf8());

  return string;
}

inline QString CommandText(const QString& string) {
  static const QMap<QString, std::function<QString()>> kCoreCommandMap = {
      {commands::kAbout, &CoreStrings::About},
      {commands::kPreferences, &CoreStrings::Settings},
      {commands::kOpen, &CoreStrings::Open},
      {commands::kQuit, &CoreStrings::Close}};

  if (kCoreCommandMap.contains(string)) {
    return kCoreCommandMap[string]();
  }

  if (commands::kMap.contains(string)) return QObject::tr(commands::kMap[string].toUtf8());

  return string;
}
}  // namespace sss::dscore::constants
