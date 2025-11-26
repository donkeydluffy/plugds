#include "Command.h"

#include <QSet>
#include <utility>

#include "ActionProxy.h"
#include "dscore/CoreConstants.h"

namespace sss::dscore {
// Helper function to check if two ContextLists have any common elements
static bool hasIntersection(const sss::dscore::ContextList& list1, const sss::dscore::ContextList& list2) {
  if (list1.isEmpty() || list2.isEmpty()) {
    return false;
  }
  QSet<int> set1(list1.begin(), list1.end());
  for (int context_id : list2) {
    if (set1.contains(context_id)) {
      return true;
    }
  }
  return false;
}

Command::Command(QString id) : action_(new ActionProxy()), id_(std::move(id)) {}

Command::~Command() { delete action_; }

auto Command::Action() -> QAction* { return action_; }

auto Command::RegisterAction(QAction* action, const sss::dscore::ContextList& visibility_contexts,
                             const sss::dscore::ContextList& enabled_contexts) -> void {
  action->setParent(this);

  // Note: ActionProxy's enabled/visible state is now managed by Command::SetContext.
  // The original action's state might be used by ActionProxy to decide its active status.
  connect(action, &QAction::changed, [action, this] { action_->setEnabled(action->isEnabled()); });

  // Store the visibility and enabled contexts for this command
  visibility_contexts_ = visibility_contexts;
  enabled_contexts_ = enabled_contexts;

  for (auto context_id : visibility_contexts) {  // Use visibility contexts to know which QActions to map
    actions_[context_id] = action;
  }
  // Also register with kGlobalContext if not already present in visibility_contexts
  if (!visibility_contexts.contains(kGlobalContext) && !actions_.contains(kGlobalContext)) {
    actions_[kGlobalContext] = action;
  }
}

auto sss::dscore::Command::SetContext(const sss::dscore::ContextList& active_contexts) -> void {
  // Determine visibility and enabled state based on the current active contexts
  const bool is_visible = hasIntersection(visibility_contexts_, active_contexts);
  const bool is_enabled = hasIntersection(enabled_contexts_, active_contexts);

  action_->setVisible(is_visible);
  action_->setEnabled(is_enabled);

  // Find the most specific QAction to set as active for the proxy
  QAction* specific_action = nullptr;
  // Iterate active_contexts to find the most specific action (assuming higher ID implies more specific)
  // Or simply iterate to find first match
  for (int current_active_context : active_contexts) {
    if (actions_.contains(current_active_context)) {
      specific_action = actions_[current_active_context];
      break;  // Found a match, use it
    }
  }

  if (specific_action == nullptr && actions_.contains(sss::dscore::kGlobalContext)) {
    specific_action = actions_[sss::dscore::kGlobalContext];
  }

  action_->SetActive(specific_action);
}

auto sss::dscore::Command::SetActive(bool state) -> void {
  // This method might need to be re-evaluated. If the command's enabled state is
  // purely context-driven, this method might become redundant or needs to
  // modify the underlying QAction directly (which is dangerous if it's shared).
  // For now, let ActionProxy handle it.
  action_->setEnabled(state);
}

auto sss::dscore::Command::Active() -> bool { return action_->isEnabled(); }

}  // namespace sss::dscore
