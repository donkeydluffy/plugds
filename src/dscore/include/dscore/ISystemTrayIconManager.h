#pragma once

#include <QPixmap>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"
#include "extsystem/IInterface.h"

namespace sss::dscore {
class ISystemTrayIcon;

/**
 * @brief       The ISystemTrayIconManager class is used to manage the lifecycle of system tray icons.
 *
 * @class       sss::dscore::ISystemTrayIconManager ISystemTrayIconManager.h <ISystemTrayIconManager>
 */
class NEDRYSOFT_CORE_DLLSPEC ISystemTrayIconManager : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the ISystemTrayIconManager instance.
   *
   * @returns     the ISystemTrayIconManager instance.
   */
  static auto GetInstance() -> ISystemTrayIconManager* { return sss::extsystem::GetObject<ISystemTrayIconManager>(); }

  /**
   * @brief       Creates a new system tray icon
   *
   * @param[in]   pixmap the pixmap to be used, if not supplied the application icon is used.
   *
   * @returns     the system tray icon instance.
   */
  virtual auto CreateIcon(const QPixmap& pixmap) -> sss::dscore::ISystemTrayIcon* = 0;

  /**
   * @brief       Creates a new system tray icon using the application icon.
   *
   * @returns     the system tray icon instance.
   */
  virtual auto CreateIcon() -> sss::dscore::ISystemTrayIcon* = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ISystemTrayIconManager, "com.nedrysoft.core.ISystemTrayIconManager/1.0.0")
