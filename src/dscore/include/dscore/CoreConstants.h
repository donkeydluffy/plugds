#pragma once

#include <QMap>
#include <QObject>
#include <QtGlobal>

namespace sss::dscore::constants {
namespace menubars {
constexpr auto kApplication = "Ds.Application";
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
constexpr auto kTop = "Ds.Top";
constexpr auto kMiddle = "Ds.Middle";
constexpr auto kBottom = "Ds.Bottom";

constexpr auto kFileNew = "File.New";
constexpr auto kFileOpen = "File.Open";
constexpr auto kFileSave = "File.Save";
constexpr auto kFileMisc = "File.Misc";
constexpr auto kFileExit = "File.Exit";
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
  if (menus::kMap.contains(string)) return QObject::tr(menus::kMap[string].toUtf8());

  return string;
}

inline QString CommandText(const QString& string) {
  if (commands::kMap.contains(string)) return QObject::tr(commands::kMap[string].toUtf8());

  return string;
}
}  // namespace sss::dscore::constants
