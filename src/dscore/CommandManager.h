#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "ActionContainer.h"
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

  auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* override;

  auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                      const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> bool override;

  auto SetContext(int context_id) -> void override;

  auto CreateMenu(const QString& identifier, IActionContainer* parent_container)
      -> sss::dscore::IActionContainer* override;
  auto CreatePopupMenu(const QString& identifier) -> sss::dscore::IActionContainer* override;
  auto CreateToolBar(const QString& identifier) -> sss::dscore::IActionContainer* override;

  auto FindMenu(const QString& identifier) -> sss::dscore::IActionContainer* override;

  auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* override;

 private Q_SLOTS:
  void onContextChanged(int new_context, int previous_context);

 private:  // NOLINT
  //! @cond

  QMap<QString, Command*> command_map_;
  QMap<QString, sss::dscore::ActionContainer*> action_container_map_;

  //! @endcond
};
}  // namespace sss::dscore
