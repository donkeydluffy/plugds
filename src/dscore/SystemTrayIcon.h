#pragma once

#include <QIcon>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QtGlobal>

#include "dscore/ISystemTrayIcon.h"

class QMenu;

namespace sss::dscore {
/**
 * @brief       The SystemTrayIcon class creates an system tray icon, on macOS this is in the menu bar and on
 *              windows it is in the notification tray area.  Behaviour and availability will differ under Linux.
 */
class NEDRYSOFT_CORE_DLLSPEC SystemTrayIcon : public sss::dscore::ISystemTrayIcon {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Constructs a new SystemTrayIcon using the application icon.
   *
   * @param[in]   parent the parent
   */
  explicit SystemTrayIcon(QObject* parent = nullptr);

  /**
   * @brief       Constructs a new SystemTrayIcon.
   *
   * @param[in]   pixmap the pixmap to use.
   * @param[in]   parent the parent
   */
  explicit SystemTrayIcon(const QPixmap& pixmap, QObject* parent = nullptr);

  /**
   * @brief       Destroys the SystemTrayIcon
   */
  ~SystemTrayIcon() override;

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
  auto SetColour(const QColor& new_colour) -> void override;

  /**
   * @brief       Returns the geometry for the system tray icon.
   *
   * @returns     the rectangle.
   */
  auto Geometry() -> QRect override;

#if defined(Q_OS_MACOS)
  /**
   * @brief       Shows the supplied menu.
   *
   * @note        This method is intended to be called from a slot connected to the clicked()
   *              signal of this system tray icon.
   *
   * @param[in]   menu the menu to show.
   */
  auto showMenu(QMenu* menu) -> void override;
#endif

 protected:
  /**
   * @brief       Creates the icon.
   */
  auto CreateIcon() -> void;

#if defined(Q_OS_MACOS)
  /**
   * @brief       Returns the menubar icon.
   *
   * @returns     the menubar icon.
   */
  auto menubarIcon() -> sss::machelper::MacMenubarIcon* override;
#endif
 private:
  //! @cond

  QIcon icon_;
  QPixmap base_pixmap_;
  QPixmap active_pixmap_;
  QMenu* context_menu_;
#if defined(Q_OS_MACOS)
  sss::machelper::MacMenubarIcon* menubar_icon_;
  sss::machelper::MacPopover* m_popover;
#else
  QSystemTrayIcon* systemTray_icon_;
#endif
  bool visible_;

  //! @endcond
};
}  // namespace sss::dscore
