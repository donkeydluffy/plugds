#pragma once

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {
/**
 * @brief       The IStatusbarManager describes a manager for status bars.
 *
 * @details     The status bar manager handles the management of the application status bar, components may use
 *              the status bar to display information about tasks or results that are running.
 *
 *              The status bar is normally context sensitive although components may created fixed entries that
 *              are shown regardless of the current application context
 *
 * @class       sss::dscore::IStatusbarManager IStatusbarManager.h <IStatusbarManager>
 */
class DS_CORE_DLLSPEC IStatusbarManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the IStatusbarManager instance.
   *
   * @returns     the IStatusbarManager instance.
   */
  static auto GetInstance() -> IStatusbarManager* { return sss::extsystem::GetTObject<IStatusbarManager>(); }

  /**
   * @brief       Shows a temporary message in the status bar.
   *
   * @param[in]   message The message to display.
   * @param[in]   timeout Time in milliseconds to show the message (0 = indefinite).
   */
  virtual void SetStatusMessage(const QString& message, int timeout) = 0;

  /**
   * @brief       Clears the current status message.
   */
  virtual void ClearStatusMessage() = 0;

  /**
   * @brief       Adds a permanent widget to the status bar.
   *
   * @param[in]   widget The widget to add (ownership is transferred to the status bar).
   * @param[in]   stretch The stretch factor.
   */
  virtual void AddPermanentWidget(QWidget* widget, int stretch) = 0;

  /**
   * @brief       Removes a permanent widget from the status bar.
   *
   * @param[in]   widget The widget to remove.
   */
  virtual void RemovePermanentWidget(QWidget* widget) = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IStatusbarManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IStatusbarManager, "sss.dscore.IStatusbarManager/1.0.0")
