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

    // 1. Sync Visual Properties: Real Action -> Proxy
    // 插件拥有真实的 action 并设置其图标/文本。代理必须反映这一点。
    setIcon(action->icon());
    setText(action->text());
    setToolTip(action->toolTip());
    setStatusTip(action->statusTip());
    setCheckable(action->isCheckable());

    // 2. Sync State Properties: Proxy -> Real Action
    // ContextManager控制代理的状态。真实动作必须服从。
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

  // 1. Real Action -> Proxy (Visuals)
  connect(action_, &QAction::changed, [this]() {
    if (icon().cacheKey() != action_->icon().cacheKey()) setIcon(action_->icon());
    if (text() != action_->text()) setText(action_->text());
    if (toolTip() != action_->toolTip()) setToolTip(action_->toolTip());
    if (statusTip() != action_->statusTip()) setStatusTip(action_->statusTip());
    if (isCheckable() != action_->isCheckable()) setCheckable(action_->isCheckable());
  });

  // 2. Proxy -> Real Action (State)
  // 当代理状态改变时（通过SetContext），同步到真实动作
  connect(this, &QAction::changed, [this]() {
    if (action_ != nullptr) {
      if (action_->isEnabled() != isEnabled()) action_->setEnabled(isEnabled());
      if (action_->isVisible() != isVisible()) action_->setVisible(isVisible());
      if (action_->isCheckable() && isCheckable() && action_->isChecked() != isChecked()) {
        action_->setChecked(isChecked());
      }
    }
  });
}
