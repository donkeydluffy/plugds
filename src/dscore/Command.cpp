#include "Command.h"

#include <utility>

#include "ActionProxy.h"

sss::dscore::Command::Command(QString id) : action_(new ActionProxy()), id_(std::move(id)) {}

sss::dscore::Command::~Command() { delete action_; }

auto sss::dscore::Command::Action() -> QAction* { return action_; }

auto sss::dscore::Command::RegisterAction(QAction* action, const sss::dscore::ContextList& contexts) -> void {
  action->setParent(this);

  connect(action, &QAction::changed, [action, this] { action_->setEnabled(action->isEnabled()); });

  for (auto context_id : contexts) {
    actions_[context_id] = action;
  }

  if (sss::dscore::IContextManager::GetInstance() != nullptr) {
    SetContext(sss::dscore::IContextManager::GetInstance()->Context());
  }
}

auto sss::dscore::Command::SetContext(int context_id) -> void {
  if (actions_.contains(context_id)) {
    action_->SetActive(actions_[context_id]);
  } else {
    if (actions_.contains(sss::dscore::kGlobalContext)) {
      action_->SetActive(actions_[sss::dscore::kGlobalContext]);
    } else {
      action_->SetActive(nullptr);
    }
  }
}

auto sss::dscore::Command::SetActive(bool state) -> void { action_->setEnabled(state); }

auto sss::dscore::Command::Active() -> bool { return action_->isEnabled(); }
