#include "ContextManager.h"

sss::dscore::ContextManager::ContextManager() : current_context_id_(0), next_context_id_(1) {}

auto sss::dscore::ContextManager::RegisterContext(QString context_identifier) -> int {
  Q_UNUSED(context_identifier)

  context_ids_[context_identifier] = next_context_id_;

  return next_context_id_++;
}

auto sss::dscore::ContextManager::SetContext(int context_identifier) -> int {
  Q_EMIT ContextChanged(context_identifier, current_context_id_);

  current_context_id_ = context_identifier;

  return 0;
}

auto sss::dscore::ContextManager::Context() -> int { return current_context_id_; }

auto sss::dscore::ContextManager::Context(QString context_name) -> int {
  if (context_ids_.contains(context_name)) {
    return context_ids_[context_name];
  }

  return 0;
}
