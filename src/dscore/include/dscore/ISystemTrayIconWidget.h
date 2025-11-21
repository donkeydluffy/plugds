#pragma once

#include "dscore/CoreSpec.h"
#include "extsystem/IInterface.h"

namespace sss::dscore {
/**
 * @brief       The ISystemTrayIconWidget is an item that is displayed the in the popup window of the
 *              operating systems system tray (on macOS this is a menu bar item)
 *
 * @class       sss::dscore::ISystemTrayIconWidget ISystemTrayIconWidget.h <ISystemTrayIconWidget>
 */
class NEDRYSOFT_CORE_DLLSPEC ISystemTrayIconWidget : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the widget for this item.
   *
   * @returns     the widget.
   */
  // virtual auto widget() -> QWidget * = nullptr;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ISystemTrayIconWidget, "com.nedrysoft.core.ISystemTrayIconWidget/1.0.0")
