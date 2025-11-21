#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"

namespace sss::dscore {
class ActionProxy;

/**
 * @brief       ICommand interface
 *
 * @details     ICommand represents an actionable command in the system, commands
 *              are bound to QActions for given contexts, this allows the target of
 *              the command to change depending on the current context that the application
 *              is in.
 */
class Command : public sss::dscore::ICommand {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICommand)

 public:
  /**
   * @brief       Constructs a new Command with the given id.
   *
   * @param[in]   id the identifier for this command.
   */
  explicit Command(QString id);

  /**
   * @brief       Destroys the Command.
   */
  ~Command() override;

  /**
   * @brief       Returns the proxy action.
   *
   * @see         sss::dscore::ICommand::action
   *
   * @returns     the proxy action
   */
  auto Action() -> QAction* override;

  /**
   * @brief       Sets the active state of the command.
   *
   * @see         sss::dscore::ICommand::setActive
   *
   * @param[in]   state true if active; otherwise false.
   */
  auto SetActive(bool state) -> void override;

  /**
   * @brief       Returns the active state of the command.
   *
   * @see         sss::dscore::ICommand::active
   *
   * @returns     true if enabled; otherwise false.
   */
  auto Active() -> bool override;

 protected:
  /**
   * @brief       Registers an action to the given contexts.
   *
   * @note        The command manager becomes the owner of the action.
   *
   * @param[in]   action the action.
   * @param[in]   contexts the list of contexts this action is used in.
   */
  auto RegisterAction(QAction* action, const sss::dscore::ContextList& contexts) -> void;

  /**
   * @brief       Sets the current context for this command.
   *
   * @brief       If there is a QAction registered with the contextId then it becomes the active
   *              active, if there is no QAction for the context then the command is disabled.
   *
   * @param[in]   contextId the context id.
   */
  auto SetContext(int context_id) -> void;

  friend class CommandManager;
  friend class RibbonBarManager;

 private:
  //! @cond

  QMap<int, QAction*> actions_;

  sss::dscore::ActionProxy* action_;
  QString id_;

  //! @endcond
};
}  // namespace sss::dscore
