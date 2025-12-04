#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"

namespace sss::dscore {
class ActionProxy;

/**
 * @brief       ICommand 接口
 *
 * @details     ICommand 代表系统中一个可执行的命令，命令绑定到给定上下文的
 *              QAction，这使得命令的目标可以根据应用程序当前所在的上下文而改变。
 */
class Command : public sss::dscore::ICommand {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICommand)

 public:
  /**
   * @brief       使用给定的ID构造新的Command。
   *
   * @param[in]   id 此命令的标识符。
   */
  explicit Command(QString id);

  /**
   * @brief       销毁Command。
   */
  ~Command() override;

  /**
   * @brief       返回代理动作。
   *
   * @see         sss::dscore::ICommand::action
   *
   * @returns     代理动作
   */
  auto Action() -> QAction* override;

  /**
   * @brief       设置命令的活动状态。
   *
   * @see         sss::dscore::ICommand::setActive
   *
   * @param[in]   state 如果活动则为true；否则为false。
   */
  auto SetActive(bool state) -> void override;

  /**
   * @brief       返回命令的活动状态。
   *
   * @see         sss::dscore::ICommand::active
   *
   * @returns     如果启用则为true；否则为false。
   */
  auto Active() -> bool override;

 protected:
  /**
   * @brief       向给定上下文注册一个动作。
   *
   * @note        命令管理器成为该动作的所有者。
   *
   * @param[in]   action 动作。
   * @param[in]   contexts 使用此动作的上下文列表。
   */
  auto RegisterAction(QAction* action, const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> void;

  /**
   * @brief       设置此命令的当前上下文。
   *
   * @brief       如果有与contextId注册的QAction，则它成为活动的；
   *              如果该上下文没有QAction，则命令被禁用。
   *
   * @param[in]   active_contexts 当前活动上下文的列表。
   */
  auto SetContext(const sss::dscore::ContextList& active_contexts) -> void;

  friend class CommandManager;
  friend class RibbonBarManager;

 private:
  //! @cond

  QMap<int, QAction*> actions_;

  sss::dscore::ContextList visibility_contexts_;
  sss::dscore::ContextList enabled_contexts_;

  sss::dscore::ActionProxy* action_;
  QString id_;

  //! @endcond
};
}  // namespace sss::dscore
