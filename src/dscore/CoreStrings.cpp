#include "dscore/CoreStrings.h"

namespace sss::dscore {

auto CoreStrings::Ok() -> QString { return tr("OK"); }
auto CoreStrings::Cancel() -> QString { return tr("Cancel"); }
auto CoreStrings::Yes() -> QString { return tr("Yes"); }
auto CoreStrings::No() -> QString { return tr("No"); }
auto CoreStrings::Apply() -> QString { return tr("Apply"); }
auto CoreStrings::Close() -> QString { return tr("Close"); }
auto CoreStrings::Save() -> QString { return tr("Save"); }
auto CoreStrings::Open() -> QString { return tr("Open"); }
auto CoreStrings::Delete() -> QString { return tr("Delete"); }
auto CoreStrings::Edit() -> QString { return tr("Edit"); }

auto CoreStrings::Error() -> QString { return tr("Error"); }
auto CoreStrings::Warning() -> QString { return tr("Warning"); }
auto CoreStrings::Information() -> QString { return tr("Information"); }
auto CoreStrings::Success() -> QString { return tr("Success"); }
auto CoreStrings::Loading() -> QString { return tr("Loading..."); }

auto CoreStrings::Settings() -> QString { return tr("Settings"); }
auto CoreStrings::Help() -> QString { return tr("Help"); }
auto CoreStrings::About() -> QString { return tr("About"); }
auto CoreStrings::File() -> QString { return tr("File"); }
auto CoreStrings::Language() -> QString { return tr("Language"); }
auto CoreStrings::Theme() -> QString { return tr("Theme"); }

}  // namespace sss::dscore
