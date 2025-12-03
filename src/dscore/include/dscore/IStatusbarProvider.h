#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class IStatusbarManager;

/**
 * @brief       Interface for components that contribute to the application status bar.
 *
 * @details     Implement this interface to add widgets or indicators to the global status bar.
 */
class DS_CORE_DLLSPEC IStatusbarProvider {
 public:
  virtual ~IStatusbarProvider() = default;

  /**
   * @brief       Contributes to the application status bar.
   *
   * @param[in]   statusbar_manager The application's status bar manager instance.
   */
  virtual void ContributeToStatusbar(IStatusbarManager* statusbar_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IStatusbarProvider, "sss.dscore.IStatusbarProvider/1.0")
