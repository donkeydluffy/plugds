#include "ws2/Ws2Strings.h"

namespace sss::ws2 {

auto Ws2Strings::SampleCommand() -> QString { return tr("Ws2 Sample Command"); }
auto Ws2Strings::SampleCommandDesc() -> QString { return tr("This is a sample command from the Ws2 plugin."); }

auto Ws2Strings::WorkspaceTitle() -> QString { return tr("Workspace 2"); }
auto Ws2Strings::ProjectBrowser() -> QString { return tr("Project Browser"); }
auto Ws2Strings::WelcomeMessage() -> QString { return tr("Welcome to Workspace 2"); }
auto Ws2Strings::RenderingArea() -> QString { return tr("Workspace 2 Area (Background)"); }
auto Ws2Strings::SystemStatus() -> QString { return tr("System Status"); }
auto Ws2Strings::ScannerIdle() -> QString { return tr("Scanner B: Idle"); }
auto Ws2Strings::StatusInfo() -> QString { return tr("Memory: 45% | CPU: 12%"); }
auto Ws2Strings::StartProcess() -> QString { return tr("Start Process"); }
auto Ws2Strings::StopProcess() -> QString { return tr("Stop Process"); }

auto Ws2Strings::Input() -> QString { return tr("Input"); }
auto Ws2Strings::Output() -> QString { return tr("Output"); }
auto Ws2Strings::Logs() -> QString { return tr("Logs"); }

}  // namespace sss::ws2
