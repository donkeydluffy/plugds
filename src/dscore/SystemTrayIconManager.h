#pragma once

#include <QMap>
// #include <RibbonWidget>

#include "dscore/ISystemTrayIconManager.h"

namespace sss::dscore {

/**
 * @brief       The SustemTrayIconManager implements ISystemTrayIconManager.
 */
class SystemTrayIconManager : public sss::dscore::ISystemTrayIconManager {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ISystemTrayIconManager)

 public:
  /**
   * @brief       Constructs a SustemTrayIconManager.
   */
  SystemTrayIconManager() = default;

  /**
   * @brief       Destroys the RibbonBarManager.
   */
  ~SystemTrayIconManager() override = default;

  /**
   * @brief       Creates a new system tray icon
   *
   * @param[in]   pixmap the pixmap to be used.
   *
   * @returns     the system tray icon instance.
   */
  auto CreateIcon(const QPixmap& pixmap) -> sss::dscore::ISystemTrayIcon* override;

  /**
   * @brief       Creates a new system tray icon using the application icon.
   *
   * @returns     the system tray icon instance.
   */
  auto CreateIcon() -> sss::dscore::ISystemTrayIcon* override;
};
}  // namespace sss::dscore
