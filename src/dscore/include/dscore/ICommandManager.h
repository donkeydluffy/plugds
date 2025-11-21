#pragma once

#include <QAction>
#include <QList>
#include <QObject>
#include <utility>

#include "dscore/CoreSpec.h"
#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"
#include "dscore/IMenu.h"
#include "extsystem/IInterface.h"

namespace sss::dscore {
/**
 * @brief       The ICommandManager interface is responsible for creating Commands and updating them when the
 *              application context changes.
 *
 * @details     It provides methods for creating menus and locating commands.
 *
 * @class       sss::dscore::ICommandManager ICommandManager.h <ICommandManager>
 */
class NEDRYSOFT_CORE_DLLSPEC ICommandManager : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Gets the sss::dscore::ICommandManager instance.
   *
   * @returns     the ICommandManager instance.
   */
  static auto GetInstance() -> ICommandManager* { return sss::extsystem::GetObject<ICommandManager>(); }

  /**
   * @brief       Registers a QAction with a command for a given context.
   *
   * @details     This function registers an action by command id, if the command already exists
   *              then the action is added for the given context, otherwise a new command is created.
   *
   * @see         sss::dscore::ICommandManager::registerAction
   *
   * @param[in]   action the action.
   * @param[in]   id the identifier of the command.
   * @param[in]   contexts the contexts this action is valid in.
   *
   * @returns     a pointer to the ICommand
   */
  virtual auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& contexts)
      -> sss::dscore::ICommand* = 0;

  /**
   * @brief       Registers an action to the given command.
   *
   * @details     This function registers an action by command id, if the command already exists
   *              then the action is added for the given context, otherwise a new command is created.
   *
   * @param[in]   action the action.
   * @param[in]   id the identifier of the command.
   * @param[in]   contextId the context this action is valid in.
   *
   * @returns     the sss::dscore::ICommand command.
   *
   */
  virtual auto RegisterAction(QAction* action, QString id, int context_id) -> sss::dscore::ICommand* {
    return RegisterAction(action, std::move(id), sss::dscore::ContextList() << context_id);
  }

  /**
   * @brief       Registers a QAction with a command for a given context.
   *
   * @details     This function registers an action to the given ICommand.
   *
   * @see         sss::dscore::ICommandManager::registerAction
   *
   * @param[in]   action the action.
   * @param[in]   command the identifier of the command.
   * @param[in]   contextId the context this action is valid in.
   *
   * @returns     true if the QAction was registered; otherwise false.
   */
  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command, int context_id) -> bool {
    return RegisterAction(action, command, sss::dscore::ContextList() << context_id);
  }

  /**
   * @brief       Registers a QAction with a command for a given context.
   *
   * @details     This function registers an action to the given ICommand.
   *
   * @see         sss::dscore::ICommandManager::registerAction
   *
   * @param[in]   action the action.
   * @param[in]   command the identifier of the command.
   * @param[in]   contexts a list of contexts this action is valid in.
   *
   * @returns     true if the QAction was registered; otherwise false.
   */
  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command, const sss::dscore::ContextList& contexts)
      -> bool = 0;

  /**
   * @brief       Sets the currently active context.
   *
   * @details     Updates all commands registered so that they connect to the correct QAction for the context.
   *
   * @param[in]   contextId the context to set as active.
   */
  virtual auto SetContext(int context_id) -> void = 0;

  /**
   * @brief       Create a menu.
   *
   * @details     Creates an IMenu object, the given identifier should be unique.
   *
   * @param[in]   identifier the unique identifier for this menu
   * @param[in]   parentMenu  if the case of a submenu, parentMenu should be set to the parent IMenu instance.
   *
   * @returns     a new IMenu instance for the menu.
   */
  virtual auto CreateMenu(const QString& identifier, IMenu* parent_menu) -> sss::dscore::IMenu* = 0;

  /**
   * @brief       Find a menu.
   *
   * @details     Finds a menu by the given identifier, application defined constants are located in
   *              the Pingnoo::Constants namespace in Pingnoo.h
   *
   * @param[in]   identifier the unique identifier for the menu.
   *
   * @returns     the IMenu instance if the menu exists; otherwise nullptr.
   */
  virtual auto FindMenu(const QString& identifier) -> sss::dscore::IMenu* = 0;

  /**
   * @brief       Create a popup menu.
   *
   * @details     Creates an IMenu object, if an identifier is not given then the menu is a transient
   *              one; otherwise the identifier should be unique.
   *
   * @param[in]   identifier the unique identifier for this menu.
   *
   * @returns     a new IMenu instance for the menu.
   */
  virtual auto CreatePopupMenu(const QString& identifier) -> sss::dscore::IMenu* = 0;

  /**
   * @brief       Find a command.
   *
   * @details     Finds a registered command by given identifier.
   *
   * @param[in]   identifier the identifier for the command.
   *
   * @returns     The ICommand instance if the command exists; otherwise nullptr;
   */
  virtual auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~ICommandManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommandManager, "com.nedrysoft.core.ICommandManager/1.0.0")
