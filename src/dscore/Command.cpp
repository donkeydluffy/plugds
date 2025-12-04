#include "Command.h"

#include <spdlog/spdlog.h>

#include <QSet>
#include <algorithm>
#include <utility>

#include "ActionProxy.h"

namespace {
// 辅助函数：检查两个 ContextList 是否有任何共同元素
bool HasIntersection(const sss::dscore::ContextList& list1, const sss::dscore::ContextList& list2) {
  if (list1.isEmpty() || list2.isEmpty()) {
    return false;
  }
  QSet<int> set1(list1.begin(), list1.end());
  return std::any_of(list2.begin(), list2.end(), [&set1](int context_id) { return set1.contains(context_id); });
}

// 辅助函数：检查所有必需的上下文是否处于活动状态（用于启用状态）
bool HasAllRequired(const sss::dscore::ContextList& required_contexts,
                    const sss::dscore::ContextList& active_contexts) {
  if (required_contexts.isEmpty()) {
    return true;  // 如果没有要求，则启用（或由交集决定）
  }
  QSet<int> active_set(active_contexts.begin(), active_contexts.end());
  return std::all_of(required_contexts.begin(), required_contexts.end(),
                     [&active_set](int context_id) { return active_set.contains(context_id); });
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
  // 原始 action 的状态可能被 ActionProxy 用来决定其活动状态。
  connect(action, &QAction::changed, [action, this] { action_->setEnabled(action->isEnabled()); });

  // 存储此命令的可见性和启用上下文
  visibility_contexts_ = visibility_contexts;
  enabled_contexts_ = enabled_contexts;

  for (auto context_id : visibility_contexts) {  // 使用可见性上下文来确定要映射哪些 QActions
    actions_[context_id] = action;
  }
  // 仅当此命令应该全局可见时才向 kGlobalContext 注册
  if (visibility_contexts.contains(kGlobalContext)) {
    actions_[kGlobalContext] = action;
  }
}

auto Command::SetContext(const sss::dscore::ContextList& active_contexts) -> void {
  // 基于当前活动上下文确定可见性和启用状态
  const bool is_visible = HasIntersection(visibility_contexts_, active_contexts);
  const bool is_enabled = HasAllRequired(enabled_contexts_, active_contexts);

  action_->setVisible(is_visible);
  action_->setEnabled(is_enabled);

  // 查找最具体的 QAction 设置为代理的活动状态
  QAction* specific_action = nullptr;
  // 逆序遍历 active_contexts 以找到最具体的 action（更高 ID = 更具体）
  for (auto it = active_contexts.rbegin(); it != active_contexts.rend(); ++it) {
    int current_active_context = *it;
    if (actions_.contains(current_active_context)) {
      specific_action = actions_[current_active_context];
      break;  // 找到匹配项，使用它
    }
  }

  // 仅在没有找到具体 action 时才回退到全局上下文
  if (specific_action == nullptr && actions_.contains(sss::dscore::kGlobalContext)) {
    specific_action = actions_[sss::dscore::kGlobalContext];
  }

  action_->SetActive(specific_action);
}

auto Command::SetActive(bool state) -> void { action_->setEnabled(state); }

auto Command::Active() -> bool { return action_->isEnabled(); }

}  // namespace sss::dscore
