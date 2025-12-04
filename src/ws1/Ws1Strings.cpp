#include "ws1/Ws1Strings.h"

namespace sss::ws1 {

auto Ws1Strings::SampleCommand() -> QString { return tr("Ws1 Sample Command"); }
auto Ws1Strings::SampleCommandDesc() -> QString { return tr("This is a sample command from the Ws1 plugin."); }

auto Ws1Strings::WorkspaceTitle() -> QString { return tr("Workspace 1"); }
auto Ws1Strings::ModelTree() -> QString { return tr("Model Tree"); }
auto Ws1Strings::WelcomeMessage() -> QString { return tr("Welcome to Workspace 1"); }
auto Ws1Strings::RenderingArea() -> QString { return tr("3D Rendering Area (Background)"); }
auto Ws1Strings::DeviceInfo() -> QString { return tr("Device Info"); }
auto Ws1Strings::ScannerReady() -> QString { return tr("Scanner A: Ready"); }
auto Ws1Strings::StatusInfo() -> QString { return tr("Temp: 45C | FPS: 60"); }
auto Ws1Strings::EnableContext() -> QString { return tr("Enable Context"); }
auto Ws1Strings::DisableContext() -> QString { return tr("Disable Context"); }

auto Ws1Strings::Reference() -> QString { return tr("Reference"); }
auto Ws1Strings::Data() -> QString { return tr("Data"); }
auto Ws1Strings::Features() -> QString { return tr("Features"); }

}  // namespace sss::ws1
