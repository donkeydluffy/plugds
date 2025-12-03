#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       Interface for components that contribute to the application toolbar.
 *
 * @details     Implement this interface to define toolbar items (buttons, separators, widgets).
 */
class DS_CORE_DLLSPEC IToolbarProvider {
 public:
  virtual ~IToolbarProvider() = default;

  /**
   * @brief       Contributes to the application toolbar structure.
   *
   * @param[in]   command_manager The application's command manager instance.
   */
  virtual void ContributeToToolbar(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IToolbarProvider, "sss.dscore.IToolbarProvider/1.0")
