#include "Command.h"

#include <QSet>
#include <utility>

#include <spdlog/spdlog.h>

#include "ActionProxy.h"

namespace {
// Helper function to check if two ContextLists have any common elements
bool HasIntersection(const sss::dscore::ContextList& list1, const sss::dscore::ContextList& list2) {
  if (list1.isEmpty() || list2.isEmpty()) {
    return false;
  }
  QSet<int> set1(list1.begin(), list1.end());
  return std::any_of(list2.begin(), list2.end(), [&set1](int context_id) { return set1.contains(context_id); });
}

// Helper function to check if all required contexts are active (for enabled state)
bool HasAllRequired(const sss::dscore::ContextList& required_contexts, const sss::dscore::ContextList& active_contexts) {
  if (required_contexts.isEmpty()) {
    return false;
  }
  QSet<int> active_set(active_contexts.begin(), active_contexts.end());
  return std::all_of(required_contexts.begin(), required_contexts.end(), [&active_set](int context_id) { return active_set.contains(context_id); });
}
}  // namespace
namespace sss::dscore {

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
  // Only register with kGlobalContext if this command should be globally visible
  if (visibility_contexts.contains(kGlobalContext)) {
    actions_[kGlobalContext] = action;
  }
}

auto sss::dscore::Command::SetContext(const sss::dscore::ContextList& active_contexts) -> void {
  if (id_ == "ws1.sample_command") {
    QString active_str;
    for (int ctx : active_contexts) {
      active_str += QString::number(ctx) + " ";
    }
    QString vis_str;
    for (int ctx : visibility_contexts_) {
      vis_str += QString::number(ctx) + " ";
    }
    QString en_str;
    for (int ctx : enabled_contexts_) {
      en_str += QString::number(ctx) + " ";
    }
    SPDLOG_INFO("Command::SetContext for ws1.sample_command");
    SPDLOG_INFO("  active_contexts: [{}]", active_str.toStdString());
    SPDLOG_INFO("  visibility_contexts: [{}]", vis_str.toStdString());
    SPDLOG_INFO("  enabled_contexts: [{}]", en_str.toStdString());
  }

  // Determine visibility and enabled state based on the current active contexts
  const bool is_visible = HasIntersection(visibility_contexts_, active_contexts);
  const bool is_enabled = HasAllRequired(enabled_contexts_, active_contexts);

  if (id_ == "ws1.sample_command") {
    SPDLOG_INFO("ws1.sample_command - is_visible: {}, is_enabled: {}", is_visible, is_enabled);
  }

  action_->setVisible(is_visible);
  action_->setEnabled(is_enabled);

  // Find the most specific QAction to set as active for the proxy
  QAction* specific_action = nullptr;
  // Iterate active_contexts in reverse order to find the most specific action (higher ID = more specific)
  for (auto it = active_contexts.rbegin(); it != active_contexts.rend(); ++it) {
    int current_active_context = *it;
    if (actions_.contains(current_active_context)) {
      specific_action = actions_[current_active_context];
      if (id_ == "ws1.sample_command") {
        SPDLOG_INFO("ws1.sample_command - found specific action for context {}", current_active_context);
      }
      break;  // Found a match, use it
    }
  }

  // Only fall back to global context if no specific action found
  if (specific_action == nullptr && actions_.contains(sss::dscore::kGlobalContext)) {
    specific_action = actions_[sss::dscore::kGlobalContext];
    if (id_ == "ws1.sample_command") {
      SPDLOG_INFO("ws1.sample_command - using global context action as fallback");
    }
  }

  action_->SetActive(specific_action);

  if (id_ == "ws1.sample_command") {
    SPDLOG_INFO("ws1.sample_command - completed, action proxy enabled: {}", action_->isEnabled());
  }
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
