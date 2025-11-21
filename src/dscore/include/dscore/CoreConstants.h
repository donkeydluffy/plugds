#pragma once

#include <QMap>
#include <QObject>
#include <QtGlobal>

namespace sss::dscore::constants {
namespace menubars {
constexpr auto kApplication = "Pingnoo.Application";
};

namespace menus {
constexpr auto kFile = "Pingnoo.File";
constexpr auto kEdit = "Pingnoo.Edit";
constexpr auto kHelp = "Pingnoo.Help";
constexpr auto kApplication = "Pingnoo.Application";

const QMap<QString, QString> kMap = {{kFile, QT_TR_NOOP("File")},
                                     {kEdit, QT_TR_NOOP("Edit")},
                                     {kHelp, QT_TR_NOOP("Help")},
                                     {kApplication, QT_TR_NOOP("Application")}};
}  // namespace menus

namespace menugroups {
constexpr auto kTop = "Pingnoo.Top";
constexpr auto kMiddle = "Pingnoo.Middle";
constexpr auto kBottom = "Pingnoo.Bottom";

constexpr auto kFileNew = "File.New";
constexpr auto kFileOpen = "File.Open";
constexpr auto kFileSave = "File.Save";
constexpr auto kFileMisc = "File.Misc";
constexpr auto kFileExit = "File.Exit";
}  // namespace menugroups

namespace commands {
constexpr auto kPreferences = "Pingnoo.Preferences";

constexpr auto kOpen = "Pingnoo.Open";
constexpr auto kQuit = "Pingnoo.Quit";

constexpr auto kCut = "Pingnoo.Cut";
constexpr auto kCopy = "Pingnoo.Copy";
constexpr auto kPaste = "Pingnoo.Paste";

constexpr auto kAbout = "Pingnoo.About";

constexpr auto kAboutComponents = "Pingnoo.AboutComponents";

constexpr auto kShowApplication = "Pingnoo.ShowApplication";
constexpr auto kHideApplication = "Pingnoo.HideApplication";

const QMap<QString, QString> kMap = {{kPreferences, QT_TR_NOOP("Preferences")},

                                     {kCut, QT_TR_NOOP("Cut")},
                                     {kCopy, QT_TR_NOOP("Copy")},
                                     {kPaste, QT_TR_NOOP("Paste")},

                                     {kOpen, QT_TR_NOOP("Open...")},
                                     {kQuit, QT_TR_NOOP("Exit")},

                                     {kAbout, QT_TR_NOOP("About Pingnoo")},
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
