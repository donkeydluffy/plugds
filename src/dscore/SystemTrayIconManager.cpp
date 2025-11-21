#include "SystemTrayIconManager.h"

#include "SystemTrayIcon.h"

auto sss::dscore::SystemTrayIconManager::CreateIcon() -> sss::dscore::ISystemTrayIcon* { return new SystemTrayIcon; }

auto sss::dscore::SystemTrayIconManager::CreateIcon(const QPixmap& pixmap) -> sss::dscore::ISystemTrayIcon* {
  return new SystemTrayIcon(pixmap);
}
