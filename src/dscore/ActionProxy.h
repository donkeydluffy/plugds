#pragma once

#include <QAction>
#include <QPointer>

namespace sss::dscore {
/**
 * @brief       The ActionProxy class is used to proxy actions depending on context.
 *
 * @details     A proxy class for a QAction, allows an Action to be proxied through, the active QAction can
 *              be switched depending on the current context of the application.
 */
class ActionProxy : public QAction {
 public:
  /**
   * @brief       Constructs a new ActionProxy instance which is a child of the parent.
   *
   * @param[in]   parent the owner object.
   */
  explicit ActionProxy(QObject* parent = nullptr);

  /**
   * @brief       Sets the currently active QAction.
   *
   * @param[in]   action the action to be proxied.
   */
  auto SetActive(QAction* action) -> void;

 protected:
  /**
   * @brief       Connects the current action to the proxy.
   */
  auto ConnectAction() -> void;

  /**
   * @brief       Disconnects the current action from the proxy.
   */
  auto DisconnectAction() -> void;

 private:
  //! @cond

  QPointer<QAction> action_;

  //! @endcond
};
}  // namespace sss::dscore
