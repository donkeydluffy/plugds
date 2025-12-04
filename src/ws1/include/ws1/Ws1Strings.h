#pragma once

#include <QObject>
#include <QString>

namespace sss::ws1 {

class Ws1Strings : public QObject {
  Q_OBJECT
 public:
  // --- Commands ---
  static auto SampleCommand() -> QString;
  static auto SampleCommandDesc() -> QString;

  // --- UI Elements ---
  static auto WorkspaceTitle() -> QString;
  static auto ModelTree() -> QString;
  static auto WelcomeMessage() -> QString;
  static auto RenderingArea() -> QString;
  static auto DeviceInfo() -> QString;
  static auto ScannerReady() -> QString;
  static auto StatusInfo() -> QString;  // "Temp: 45C | FPS: 60"
  static auto EnableContext() -> QString;
  static auto DisableContext() -> QString;

  // --- Tree Items ---
  static auto Reference() -> QString;
  static auto Data() -> QString;
  static auto Features() -> QString;
};

}  // namespace sss::ws1
