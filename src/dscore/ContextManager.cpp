#include "ContextManager.h"

#include <spdlog/spdlog.h>

sss::dscore::ContextManager::ContextManager() : next_context_id_(1) {
  // Global context is always active and has level kGlobal
  active_contexts_.append(kGlobalContext);
  context_levels_[kGlobalContext] = ContextLevel::kGlobal;
}

auto sss::dscore::ContextManager::RegisterContext(QString context_identifier) -> int {
  // Default to Local level for backward compatibility
  return RegisterContext(context_identifier, ContextLevel::kLocal);
}

auto sss::dscore::ContextManager::RegisterContext(QString context_identifier, ContextLevel level) -> int {
  if (context_ids_.contains(context_identifier)) {
    return context_ids_[context_identifier];
  }
  int id = next_context_id_++;
  context_ids_[context_identifier] = id;
  context_levels_[id] = level;
  SPDLOG_INFO("RegisterContext: '{}' -> id={}, level={}", context_identifier.toStdString(), id, static_cast<int>(level));
  return id;
}

auto sss::dscore::ContextManager::SetContext(int context_identifier) -> int {
  // This method activates a workspace context (for backward compatibility)
  ActivateWorkspace(context_identifier);
  return 0;
}

auto sss::dscore::ContextManager::ActivateWorkspace(int context_id) -> void {
  SPDLOG_INFO("ContextManager::ActivateWorkspace called with context_id: {}", context_id);

  if (context_id == kGlobalContext) {
    SPDLOG_INFO("ActivateWorkspace: ignoring kGlobalContext");
    return;
  }

  int old_workspace = active_workspace_;
  int old_primary_context = Context();

  // Deactivate old workspace if different
  if (old_workspace != kGlobalContext && old_workspace != context_id) {
    DeactivateWorkspace(old_workspace);
  }

  // Set new active workspace
  active_workspace_ = context_id;

  // Add workspace context to active list if not already present
  if (!active_contexts_.contains(context_id)) {
    active_contexts_.append(context_id);
  }

  SPDLOG_INFO("ActivateWorkspace: activated workspace {}, active_contexts size: {}",
              context_id, active_contexts_.size());

  Q_EMIT WorkspaceChanged(context_id, old_workspace);
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::DeactivateWorkspace(int context_id) -> void {
  SPDLOG_INFO("ContextManager::DeactivateWorkspace called with context_id: {}", context_id);

  if (context_id == kGlobalContext) {
    SPDLOG_INFO("DeactivateWorkspace: ignoring kGlobalContext");
    return;
  }

  int old_primary_context = Context();

  // Remove all sub-contexts associated with this workspace
  QList<int> sub_contexts = workspace_subcontexts_.values(context_id);
  for (int sub_context : sub_contexts) {
    active_contexts_.removeAll(sub_context);
    SPDLOG_INFO("DeactivateWorkspace: removed sub-context {}", sub_context);
  }

  // Remove the workspace context itself
  active_contexts_.removeAll(context_id);

  // Clear active workspace if it was this one
  if (active_workspace_ == context_id) {
    active_workspace_ = kGlobalContext;
  }

  SPDLOG_INFO("DeactivateWorkspace: deactivated workspace {}, active_contexts size: {}",
              context_id, active_contexts_.size());

  Q_EMIT ContextChanged(Context(), old_primary_context);
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
  SPDLOG_INFO("AddActiveContext: added context {}, new primary: {}, active_contexts size: {}",
              context_id, Context(), active_contexts_.size());
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
  SPDLOG_INFO("RemoveActiveContext: removed context {}, new primary: {}, active_contexts size: {}",
              context_id, Context(), active_contexts_.size());
  Q_EMIT ContextChanged(Context(), old_primary_context);
}

auto sss::dscore::ContextManager::AssociateWithWorkspace(int sub_context_id, int workspace_context_id) -> void {
  SPDLOG_INFO("ContextManager::AssociateWithWorkspace: sub={} -> workspace={}", sub_context_id, workspace_context_id);
  workspace_subcontexts_.insert(workspace_context_id, sub_context_id);
}

auto sss::dscore::ContextManager::Context() -> int {
  // Returns the most recently added non-global context, or global if none exists.
  if (active_contexts_.size() > 1) {
    return active_contexts_.last();
  }
  return kGlobalContext;
}

auto sss::dscore::ContextManager::ActiveWorkspace() -> int {
  return active_workspace_;
}

auto sss::dscore::ContextManager::GetActiveContexts() const -> sss::dscore::ContextList { return active_contexts_; }

auto sss::dscore::ContextManager::GetContextLevel(int context_id) -> ContextLevel {
  if (context_levels_.contains(context_id)) {
    return context_levels_[context_id];
  }
  return ContextLevel::kLocal;  // Default to Local for unknown contexts
}

auto sss::dscore::ContextManager::Context(QString context_name) -> int {
  if (context_ids_.contains(context_name)) {
    return context_ids_[context_name];
  }
  return 0;
}
