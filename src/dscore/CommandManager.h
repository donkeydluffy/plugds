#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "ActionContainer.h"
#include "dscore/ICommandManager.h"

namespace sss::dscore {
class Command;

/**
 * @brief       CommandManager 类负责创建命令并在应用程序上下文更改时更新它们。
 *
 * @details     它提供了创建菜单和定位命令的方法。
 * @class       sss::dscore::CommandManager CommandManager.h <CommandManager>
 */
class CommandManager : public sss::dscore::ICommandManager {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICommandManager)

 public:
  /**
   * @brief       构造一个新的 CommandManager。
   */
  CommandManager();

  /**
   * @brief       销毁 CommandManager。
   */
  ~CommandManager() override;

  auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* override;

  auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                      const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> bool override;

  auto SetContext(int context_id) -> void override;

  auto CreateActionContainer(const QString& identifier, sss::dscore::ContainerType type,
                             IActionContainer* parent_container, int order) -> sss::dscore::IActionContainer* override;

  auto FindContainer(const QString& identifier) -> sss::dscore::IActionContainer* override;

  auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* override;

  auto RetranslateUi() -> void override;

 private slots:
  void onContextChanged(int new_context, int previous_context);

 private:  // NOLINT
  auto createMenu(const QString& identifier, IActionContainer* parent_container, int order)
      -> sss::dscore::IActionContainer*;
  auto createToolBar(const QString& identifier, int order) -> sss::dscore::IActionContainer*;

 private:  // NOLINT
  //! @cond

  QMap<QString, Command*> command_map_;
  QMap<QString, sss::dscore::ActionContainer*> action_container_map_;

  //! @endcond
};
}  // namespace sss::dscore
