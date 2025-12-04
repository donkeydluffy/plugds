#pragma once

#include <QAction>
#include <QPointer>

namespace sss::dscore {
/**
 * @brief       ActionProxy 类用于根据上下文代理动作。
 *
 * @details     QAction 的代理类，允许动作通过代理，活动的 QAction 可以根据
 *              应用程序的当前上下文进行切换。
 * @class       sss::dscore::ActionProxy ActionProxy.h <ActionProxy>
 */
class ActionProxy : public QAction {
 public:
  /**
   * @brief       构造一个新的 ActionProxy 实例，它是父对象的子对象。
   *
   * @param[in]   parent 所有者对象。
   */
  explicit ActionProxy(QObject* parent = nullptr);

  /**
   * @brief       设置当前活动的 QAction。
   *
   * @param[in]   action 要代理的动作。
   */
  auto SetActive(QAction* action) -> void;

  /**
   * @brief       重写 setEnabled 以确保状态同步。
   */
  auto setEnabled(bool enabled) -> void;

  /**
   * @brief       重写 setVisible 以确保状态同步。
   */
  auto setVisible(bool visible) -> void;

 protected:
  /**
   * @brief       将当前动作连接到代理。
   */
  auto ConnectAction() -> void;

  /**
   * @brief       从代理断开当前动作的连接。
   */
  auto DisconnectAction() -> void;

 private:
  //! @cond

  QPointer<QAction> action_;

  //! @endcond
};
}  // namespace sss::dscore
