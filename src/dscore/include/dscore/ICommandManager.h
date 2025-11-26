#pragma once

#include <QAction>
#include <QList>
#include <QObject>
#include <utility>

#include "dscore/IActionContainer.h"
#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"

namespace sss::dscore {
/**
 * @brief       The ICommandManager interface is responsible for creating Commands and updating them when the
 *              application context changes.
 *
 * @details     It provides methods for creating menus and locating commands.
 *
 * @class       sss::dscore::ICommandManager ICommandManager.h <ICommandManager>
 */
class DS_CORE_DLLSPEC ICommandManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Gets the sss::dscore::ICommandManager instance.
   *
   * @returns     the ICommandManager instance.
   */
  static auto GetInstance() -> ICommandManager* { return sss::extsystem::GetTObject<ICommandManager>(); }

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
  virtual auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                              const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* = 0;

  virtual auto RegisterAction(QAction* action, QString id, int context_id) -> sss::dscore::ICommand* {
    sss::dscore::ContextList single_context_list;
    single_context_list << context_id;
    return RegisterAction(action, std::move(id), single_context_list, single_context_list);
  }

  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                              const sss::dscore::ContextList& visibility_contexts,
                              const sss::dscore::ContextList& enabled_contexts) -> bool = 0;

  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command, int context_id) -> bool {
    sss::dscore::ContextList single_context_list;
    single_context_list << context_id;
    return RegisterAction(action, command, single_context_list, single_context_list);
  }

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
   * @details     Creates an IActionContainer object, the given identifier should be unique.
   *
   * @param[in]   identifier the unique identifier for this menu
   * @param[in]   parentMenu  if the case of a submenu, parentMenu should be set to the parent IActionContainer
   * instance.
   *
   * @returns     a new IActionContainer instance for the menu.
   */
  virtual auto CreateMenu(const QString& identifier, IActionContainer* parent_menu)
      -> sss::dscore::IActionContainer* = 0;

  /**
   * @brief       Find a menu.
   *
   * @details     Finds a menu by the given identifier, application defined constants are located in
   *              the Pingnoo::Constants namespace in Pingnoo.h
   *
   * @param[in]   identifier the unique identifier for the menu.
   *
   * @returns     the IActionContainer instance if the menu exists; otherwise nullptr.
   */
  virtual auto FindMenu(const QString& identifier) -> sss::dscore::IActionContainer* = 0;

  /**
   * @brief       Create a popup menu.
   *
   * @details     Creates an IActionContainer object, if an identifier is not given then the menu is a transient
   *              one; otherwise the identifier should be unique.
   *
   * @param[in]   identifier the unique identifier for this menu.
   *
   * @returns     a new IActionContainer instance for the menu.
   */
  virtual auto CreatePopupMenu(const QString& identifier) -> sss::dscore::IActionContainer* = 0;

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

  /**
   * @brief       Create a tool bar.
   *
   * @details     Creates an IActionContainer object representing a tool bar.
   *
   * @param[in]   identifier the unique identifier for this tool bar
   *
   * @returns     a new IActionContainer instance for the tool bar.
   */
  virtual auto CreateToolBar(const QString& identifier) -> sss::dscore::IActionContainer* = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~ICommandManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommandManager, "sss.dscore.ICommandManager/1.0.0")
