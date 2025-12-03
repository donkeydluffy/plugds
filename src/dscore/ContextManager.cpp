#include "ContextManager.h"

#include <spdlog/spdlog.h>

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

  // SetContext implies a hard reset of the current mode
  current_mode_context_id_ = context_identifier;
  mode_sub_contexts_storage_[current_mode_context_id_].clear();

  active_contexts_.clear();
  active_contexts_.append(kGlobalContext);
  if (context_identifier != kGlobalContext) {
    active_contexts_.append(context_identifier);
  }
  Q_EMIT ContextChanged(context_identifier, old_primary_context);
  return 0;
}

auto sss::dscore::ContextManager::AddActiveContext(int context_id) -> void {
  SPDLOG_INFO("ContextManager::AddActiveContext called with context_id: {}", context_id);
  if (context_id == kGlobalContext) {
    SPDLOG_INFO("AddActiveContext: ignoring kGlobalContext");
    return;
  }
  if (active_contexts_.contains(context_id)) {
    SPDLOG_INFO("AddActiveContext: context {} already active", context_id);
    return;
  }
  int old_primary_context = Context();
  active_contexts_.append(context_id);

  // Sync with storage: If we are in a mode, track this sub-context
  if (current_mode_context_id_ != kGlobalContext && context_id != current_mode_context_id_) {
    mode_sub_contexts_storage_[current_mode_context_id_].insert(context_id);
  }

  SPDLOG_INFO("AddActiveContext: added context {}, new primary: {}, active_contexts size: {}", context_id, Context(),
              active_contexts_.size());
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::RemoveActiveContext(int context_id) -> void {
  SPDLOG_INFO("ContextManager::RemoveActiveContext called with context_id: {}", context_id);
  if (context_id == kGlobalContext) {
    SPDLOG_INFO("RemoveActiveContext: ignoring kGlobalContext");
    return;
  }
  if (!active_contexts_.contains(context_id)) {
    SPDLOG_INFO("RemoveActiveContext: context {} not active", context_id);
    return;
  }
  int old_primary_context = Context();
  active_contexts_.removeAll(context_id);

  // Sync with storage
  if (current_mode_context_id_ != kGlobalContext) {
    mode_sub_contexts_storage_[current_mode_context_id_].remove(context_id);
  }

  SPDLOG_INFO("RemoveActiveContext: removed context {}, new primary: {}, active_contexts size: {}", context_id,
              Context(), active_contexts_.size());
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::ActivateMode(int mode_context_id) -> void {
  if (mode_context_id == current_mode_context_id_) {
    return;
  }

  // 1. Save current sub-contexts for the old mode
  // We filter out the global context and the old mode context itself
  QSet<int> current_subs;
  for (int ctx : active_contexts_) {
    if (ctx != kGlobalContext && ctx != current_mode_context_id_) {
      current_subs.insert(ctx);
    }
  }
  mode_sub_contexts_storage_[current_mode_context_id_] = current_subs;

  int old_primary_context = Context();

  // 2. Switch Mode
  current_mode_context_id_ = mode_context_id;

  // 3. Restore new mode contexts
  active_contexts_.clear();
  active_contexts_.append(kGlobalContext);
  if (mode_context_id != kGlobalContext) {
    active_contexts_.append(mode_context_id);
  }

  // Restore sub-contexts if they exist
  if (mode_sub_contexts_storage_.contains(mode_context_id)) {
    for (int sub_ctx : mode_sub_contexts_storage_[mode_context_id]) {
      if (!active_contexts_.contains(sub_ctx)) {
        active_contexts_.append(sub_ctx);
      }
    }
  }

  SPDLOG_INFO("ActivateMode: switched to context {}, active count: {}", mode_context_id, active_contexts_.size());
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::Context() -> int {
  // Returns the most recently added non-global context, or global if none exists.
  if (active_contexts_.size() > 1) {
    return active_contexts_.last();
  }
  return kGlobalContext;
}

auto sss::dscore::ContextManager::GetActiveContexts() const -> sss::dscore::ContextList { return active_contexts_; }

auto sss::dscore::ContextManager::Context(QString context_name) -> int {
  if (context_ids_.contains(context_name)) {
    return context_ids_[context_name];
  }
  return 0;
}
