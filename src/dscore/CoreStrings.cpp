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

auto CoreStrings::Error() -> QString { return tr("Error"); }
auto CoreStrings::Warning() -> QString { return tr("Warning"); }
auto CoreStrings::Information() -> QString { return tr("Information"); }
auto CoreStrings::Success() -> QString { return tr("Success"); }
auto CoreStrings::Loading() -> QString { return tr("Loading..."); }

auto CoreStrings::Settings() -> QString { return tr("Settings"); }
auto CoreStrings::Help() -> QString { return tr("Help"); }
auto CoreStrings::About() -> QString { return tr("About"); }
auto CoreStrings::Language() -> QString { return tr("Language"); }
auto CoreStrings::EnglishLanguage() -> QString { return "English"; };
auto CoreStrings::ChineseLanguage() -> QString { return "中文"; };
auto CoreStrings::Theme() -> QString { return tr("Theme"); }
auto CoreStrings::DarkTheme() -> QString { return tr("Dark Theme"); }
auto CoreStrings::LightTheme() -> QString { return tr("Light Theme"); }

auto CoreStrings::CpuLabel() -> QString { return tr("CPU: --.--%"); }
auto CoreStrings::MemLabel() -> QString { return tr("MEM: --.--%"); }
auto CoreStrings::CpuValue(double value) -> QString { return tr("CPU: %1%").arg(value, 5, 'f', 1); }
auto CoreStrings::MemValue(double value) -> QString { return tr("MEM: %1%").arg(value, 0, 'f', 1); }
auto CoreStrings::MemValueInit() -> QString { return tr("MEM: -%"); }
auto CoreStrings::MemTooltip(uint64_t used, uint64_t total, uint64_t swap_used, uint64_t swap_total) -> QString {
  return tr("RAM: %1 / %2 MB\nSwap: %3 / %4 MB").arg(used).arg(total).arg(swap_used).arg(swap_total);
}
// 假设DWORD可用或使用uint32_t。由于此cpp文件可能不包含windows.h，我们使用
// uint32_t/unsigned long。实际上，CoreStrings.cpp包含CoreStrings.h，后者包含QObject。为了类型安全和避免
// windows.h依赖（如果可能），我们坚持使用标准类型。DWORD在Windows上是unsigned long。
auto CoreStrings::MemTooltipWin(uint32_t load, uint64_t virt_used, uint64_t virt_total) -> QString {
  return tr("Physical Load: %1%\nVirtual Usage: %2 / %3 MB").arg(load).arg(virt_used).arg(virt_total);
}

}  // namespace sss::dscore
