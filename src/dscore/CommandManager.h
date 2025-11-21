#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "Menu.h"
#include "dscore/ICommandManager.h"

namespace sss::dscore {
class Command;

/**
 * @brief       The CommandManager class is responsible for creating Commands and updating them when the
 *              application context changes.
 *
 * @details     It provides methods for creating menus and locating commands.
 */
class CommandManager : public sss::dscore::ICommandManager {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICommandManager)

 public:
  /**
   * @brief       Constructs a new CommandManager.
   */
  CommandManager();

  /**
   * @brief       Destroys the CommandManager.
   */
  ~CommandManager() override;

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
  auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& contexts)
      -> sss::dscore::ICommand* override;

  /**
   * @brief       Registers a QAction with a command for a given context.
   *
   * @details     This function registers an action to the given ICommand.
   *
   * @see         sss::dscore::ICommandManager::registerAction
   *
   * @param[in]   action the action.
   * @param[in]   command the identifier of the command.
   * @param[in]   contexts the list of contexts this action is valid in.
   *
   * @returns     true if the QAction was registered; otherwise false.
   */
  auto RegisterAction(QAction* action, sss::dscore::ICommand* command, const sss::dscore::ContextList& contexts)
      -> bool override;

  /**
   * @brief       Sets the currently active context.
   *
   * @details     Updates all commands registered so that they connect to the correct QAction for the context.
   *
   * @see         sss::dscore::ICommandManager::setContext
   *
   * @param[in]   contextId the context to set as active.
   */
  auto SetContext(int context_id) -> void override;

  /**
   * @brief       Create a menu.
   *
   * @details     Creates an IMenu object, the given identifier should be unique.
   *
   * @see         sss::dscore::ICommandManager::createMenu
   *
   * @param[in]   identifier the unique identifier for this menu
   * @param[in]   parentMenu  if the case of a submenu, parentMenu should be set to the parent IMenu instance.
   *
   * @returns     a new IMenu instance for the menu.
   */
  auto CreateMenu(const QString& identifier, IMenu* parent_menu) -> sss::dscore::IMenu* override;

  /**
   * @brief       Create a popup menu.
   *
   * @details     Creates an IMenu object, the given identifier should be unique.
   *
   * @see         sss::dscore::ICommandManager::createPopupMenu
   *
   * @param[in]   identifier the unique identifier for this menu
   *
   * @returns     a new IMenu instance for the menu.
   */
  auto CreatePopupMenu(const QString& identifier) -> sss::dscore::IMenu* override;

  /**
   * @brief       Find a menu.
   *
   * @details     Finds a menu by the given identifier, application defined constants are located in
   *              the Pingnoo::Constants namespace in Pingnoo.h
   *
   * @see         sss::dscore::ICommandManager::createMenu
   *
   * @param[in]   identifier the unique identifier for the menu.
   *
   * @returns     the IMenu instance if the menu exists; otherwise nullptr.
   */
  auto FindMenu(const QString& identifier) -> sss::dscore::IMenu* override;

  /**
   * @brief       Find a command.
   *
   * @details     Finds a registered command by given identifier.
   *
   * @see         sss::dscore::ICommandManager::createMenu
   *
   * @param[in]   identifier the identifier for the command.
   *
   * @returns     The ICommand instance if the command exists; otherwise nullptr;
   */
  auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* override;

 private:
  //! @cond

  QMap<QString, Command*> command_map_;
  QMap<QString, sss::dscore::Menu*> menu_map_;

  //! @endcond
};
}  // namespace sss::dscore
