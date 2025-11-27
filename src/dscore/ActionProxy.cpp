#include "ActionProxy.h"

#include <spdlog/spdlog.h>

sss::dscore::ActionProxy::ActionProxy(QObject* parent) : QAction(parent), action_(nullptr) {}

auto sss::dscore::ActionProxy::setEnabled(bool enabled) -> void {
  QAction::setEnabled(enabled);
  // 同步到被代理的action
  if (action_ != nullptr) {
    action_->setEnabled(enabled);
  }
}

auto sss::dscore::ActionProxy::setVisible(bool visible) -> void {
  QAction::setVisible(visible);
  // 同步到被代理的action
  if (action_ != nullptr) {
    action_->setVisible(visible);
  }
}

auto sss::dscore::ActionProxy::SetActive(QAction* action) -> void {
  if (action == action_) {
    return;
  }

  DisconnectAction();

  action_ = action;

  if (action != nullptr) {
    setMenuRole(action->menuRole());
    // 注意：不要在这里同步状态！
    // ActionProxy 的状态应该由 Command::SetContext 控制
    // 被代理的 action 状态应该由 ActionProxy 控制，而不是反过来
    // 移除以下几行：
    // setEnabled(action->isEnabled());
    // setVisible(action->isVisible());
    // if (action->isCheckable()) { setChecked(action->isChecked()); }

    // 但我们需要同步当前 ActionProxy 的状态到新的 action
    action->setEnabled(isEnabled());
    action->setVisible(isVisible());
    if (action->isCheckable() && isCheckable()) {
      action->setChecked(isChecked());
    }
  }

  ConnectAction();
}

auto sss::dscore::ActionProxy::DisconnectAction() -> void {
  if (action_ == nullptr) {
    return;
  }

  disconnect(this, &QAction::triggered, action_, &QAction::triggered);
  disconnect(this, &QAction::toggled, action_, &QAction::setChecked);
  // 断开状态同步的连接 - 不需要特定的断开，会在对象销毁时自动清理
}

auto sss::dscore::ActionProxy::ConnectAction() -> void {
  if (action_ == nullptr) {
    return;
  }

  connect(this, &QAction::triggered, action_, &QAction::triggered);
  connect(this, &QAction::toggled, action_, &QAction::setChecked);

  // 建立状态同步：当ActionProxy状态改变时，同步到原始action
  connect(this, &QAction::changed, [this]() {
    if (action_ != nullptr) {
      action_->setEnabled(isEnabled());
      action_->setVisible(isVisible());
      if (action_->isCheckable() && isCheckable()) {
        action_->setChecked(isChecked());
      }
    }
  });
}
