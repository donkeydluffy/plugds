#pragma once

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"
#include "extsystem/IInterface.h"

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
class DS_CORE_DLLSPEC IStatusbarManager : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the IStatusbarManager instance.
   *
   * @returns     the IStatusbarManager instance.
   */
  static auto GetInstance() -> IStatusbarManager* { return sss::extsystem::GetObject<IStatusbarManager>(); }
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IStatusbarManager, "com.nedrysoft.core.IStatusbarManager/1.0.0")
