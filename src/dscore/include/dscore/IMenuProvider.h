#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       Interface for components that contribute to the application menu.
 *
 * @details     Implement this interface to define the menu structure (menus, groups) and place
 *              registered commands into them.
 */
class DS_CORE_DLLSPEC IMenuProvider {
 public:
  virtual ~IMenuProvider() = default;

  /**
   * @brief       Contributes to the application menu structure.
   *
   * @param[in]   command_manager The application's command manager instance, used to find containers and commands.
   */
  virtual void ContributeToMenu(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMenuProvider, "sss.dscore.IMenuProvider/1.0")
