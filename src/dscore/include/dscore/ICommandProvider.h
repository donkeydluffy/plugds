#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       Interface for components that provide commands/actions to the application.
 *
 * @details     Implement this interface to register logical commands (actions) with the CommandManager.
 *              This separates the definition of a command from its placement in menus or toolbars.
 */
class DS_CORE_DLLSPEC ICommandProvider {
 public:
  virtual ~ICommandProvider() = default;

  /**
   * @brief       Registers commands with the command manager.
   *
   * @param[in]   command_manager The application's command manager instance.
   */
  virtual void RegisterCommands(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommandProvider, "sss.dscore.ICommandProvider/1.0")
