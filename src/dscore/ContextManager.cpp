#include "ContextManager.h"

#include "dscore/CoreConstants.h"

sss::dscore::ContextManager::ContextManager() : next_context_id_(1) { active_contexts_.append(kGlobalContext); }

auto sss::dscore::ContextManager::RegisterContext(QString context_identifier) -> int {
  if (context_ids_.contains(context_identifier)) {
    return context_ids_[context_identifier];
  }
  context_ids_[context_identifier] = next_context_id_;
  return next_context_id_++;
}

auto sss::dscore::ContextManager::SetContext(int context_identifier) -> int {
  int old_primary_context = Context();
  active_contexts_.clear();
  active_contexts_.append(kGlobalContext);
  if (context_identifier != kGlobalContext) {
    active_contexts_.append(context_identifier);
  }
  Q_EMIT ContextChanged(context_identifier, old_primary_context);
  return 0;
}

auto sss::dscore::ContextManager::AddActiveContext(int context_id) -> void {
  if (context_id == kGlobalContext || active_contexts_.contains(context_id)) {
    return;
  }
  int old_primary_context = Context();
  active_contexts_.append(context_id);
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::RemoveActiveContext(int context_id) -> void {
  if (context_id == kGlobalContext || !active_contexts_.contains(context_id)) {
    return;
  }
  int old_primary_context = Context();
  active_contexts_.removeAll(context_id);
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::Context() -> int {
  // Returns the most recently added non-global context, or global if none exists.
  if (active_contexts_.size() > 1) {
    return active_contexts_.last();
  }
  return kGlobalContext;
}

auto sss::dscore::ContextManager::getActiveContexts() const -> sss::dscore::ContextList { return active_contexts_; }

auto sss::dscore::ContextManager::Context(QString context_name) -> int {
  if (context_ids_.contains(context_name)) {
    return context_ids_[context_name];
  }
  return 0;
}
