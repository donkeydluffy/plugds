#pragma once

#include <QObject>
#include <QString>

namespace sss::ws2 {

class Ws2Strings : public QObject {
  Q_OBJECT
 public:
  // --- Commands ---
  static auto SampleCommand() -> QString;
  static auto SampleCommandDesc() -> QString;

  // --- UI Elements ---
  static auto WorkspaceTitle() -> QString;
  static auto ProjectBrowser() -> QString;
  static auto WelcomeMessage() -> QString;
  static auto RenderingArea() -> QString;
  static auto SystemStatus() -> QString;
  static auto ScannerIdle() -> QString;
  static auto StatusInfo() -> QString;  // "Memory: 45% | CPU: 12%"
  static auto StartProcess() -> QString;
  static auto StopProcess() -> QString;

  // --- Tree Items ---
  static auto Input() -> QString;
  static auto Output() -> QString;
  static auto Logs() -> QString;
};

}  // namespace sss::ws2
