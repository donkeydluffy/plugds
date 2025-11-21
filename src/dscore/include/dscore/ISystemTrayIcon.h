#pragma once

#include "dscore/CoreSpec.h"
#include "extsystem/IInterface.h"

class QMenu;

namespace sss::machelper {
class MacMenubarIcon;
}

namespace sss::dscore {
/**
 * @brief       The ISystemTrayIcon represents a single system tray icon, the application (depending on OS
 *              support may have more than 1 icon.
 *
 * @class       sss::dscore::ISystemTrayIcon ISystemTrayIcon.h <ISystemTrayIcon>
 */
class NEDRYSOFT_CORE_DLLSPEC ISystemTrayIcon : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  enum class MouseButton : uint8_t { kLeft, kRight };

  /**
   * @brief       Sets whether the system tray icon is visible.
   *
   * @param[in]   visible if true the icon is shown; otherwise false.
   */
  auto SetVisible(bool visible) -> void;

  /**
   * @brief       Sets the colour of the system tray icon.
   *
   * @note        The operating system may not honour the colour due to the actual window manager
   *              and theming.  Each pixel is mixed with the selected colour, setting the alpha
   *              will set the transparency of the resulting image.
   *
   * @param[in]   newColour the desired colour.
   */
  virtual auto SetColour(const QColor& new_colour) -> void = 0;

  /**
   * @brief       Returns the geometry for the system tray icon.
   *
   * @returns     the rectangle.
   */
  virtual auto Geometry() -> QRect = 0;

  /**
   * @brief       This signal is emitted when the user clicks on the system tray icon.
   *
   * @param[in]   button the mouse button that caused the signal.
   */
  Q_SIGNAL void Clicked(const sss::dscore::ISystemTrayIcon::MouseButton& button);

#if defined(Q_OS_MACOS)
  /**
   * @brief       Shows the supplied menu.
   *
   * @note        This method is intended to be called from a slot connected to the clicked()
   *              signal of this system tray icon.
   *
   * @param[in]   menu the menu to show.
   */
  virtual auto showMenu(QMenu* menu) -> void = 0;

  /**
   * @brief       THis signal is emitted when the menu is closed.
   *
   * @param[in]   menu the menu that was closed.
   */
  Q_SIGNAL void menuClosed(QMenu* menu);
#endif
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ISystemTrayIcon, "com.nedrysoft.core.ISystemTrayIcon/1.0.0")
