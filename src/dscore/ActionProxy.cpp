#include "ActionProxy.h"

sss::dscore::ActionProxy::ActionProxy(QObject* parent) : QAction(parent), action_(nullptr) {}

auto sss::dscore::ActionProxy::SetActive(QAction* action) -> void {
  if (action == action_) {
    return;
  }

  DisconnectAction();

  action_ = action;

  if (action != nullptr) {
    setMenuRole(action->menuRole());
  }

  ConnectAction();
}

auto sss::dscore::ActionProxy::DisconnectAction() -> void {
  if (action_ == nullptr) {
    return;
  }

  disconnect(this, &QAction::triggered, action_, &QAction::triggered);
  disconnect(this, &QAction::toggled, action_, &QAction::setChecked);
}

auto sss::dscore::ActionProxy::ConnectAction() -> void {
  if (action_ == nullptr) {
    return;
  }

  connect(this, &QAction::triggered, action_, &QAction::triggered);
  connect(this, &QAction::toggled, action_, &QAction::setChecked);
}
