#include "ModeManager.h"

#include <spdlog/spdlog.h>

#include "dscore/IContextManager.h"
#include "dscore/IMode.h"
#include "dscore/IWorkbench.h"

namespace sss::dscore {

ModeManager::ModeManager(QObject* parent) {
  setParent(parent);
  SPDLOG_INFO("ModeManager created.");
}

ModeManager::~ModeManager() = default;

void ModeManager::AddMode(IMode* mode) {
  if (mode == nullptr) return;
  modes_.insert(mode->Id(), mode);
  SPDLOG_INFO("Registered Mode: {} ({})", mode->Title().toStdString(), mode->Id().toStdString());
  emit ModeAdded(mode);

  if (workbench_ != nullptr) {
    workbench_->AddModeButton(mode->Id(), mode->Title(), mode->Icon());
  }

  // 如果这是第一个模式，是否激活它？还是等待显式激活。
  // 等待通过UI或配置进行显式激活。
}

void ModeManager::ActivateMode(const QString& id) {
  if (!modes_.contains(id)) {
    SPDLOG_WARN("Attempted to activate unknown mode: {}", id.toStdString());
    return;
  }

  IMode* new_mode = modes_[id];
  if (active_mode_ == new_mode) return;

  IMode* old_mode = active_mode_;
  // 记录用户切换工作空间的关键操作
  SPDLOG_INFO("用户切换工作空间: {} -> {}",
              old_mode ? old_mode->Title().toStdString() : "无",
              new_mode->Title().toStdString());

  // 1. 停用旧模式
  if (old_mode != nullptr) {
    old_mode->Deactivate();
  }

  // 2. Update Active Mode Pointer
  active_mode_ = new_mode;

  // 3. 激活新模式
  if (active_mode_ != nullptr) {
    // Set Context (Switch Mode)
    auto* cm = IContextManager::GetInstance();
    if (cm != nullptr) {
      cm->ActivateMode(active_mode_->ContextId());
    }

    // 设置工作台内容
    if (workbench_ != nullptr) {
      workbench_->Clear();
      workbench_->SetActiveModeButton(active_mode_->Id());
      active_mode_->Activate();
    }
  }

  emit ModeChanged(active_mode_, old_mode);
}

IMode* ModeManager::ActiveMode() const { return active_mode_; }

QList<IMode*> ModeManager::Modes() const { return modes_.values(); }

void ModeManager::SetGlobalWorkbench(IWorkbench* workbench) {
  workbench_ = workbench;
  if (workbench_ != nullptr) {
    workbench_->SetModeSwitchCallback([this](const QString& id) { ActivateMode(id); });

    for (auto* mode : modes_) {
      workbench_->AddModeButton(mode->Id(), mode->Title(), mode->Icon());
    }

    if (active_mode_ != nullptr) {
      workbench_->SetActiveModeButton(active_mode_->Id());
    }
  }
}

}  // namespace sss::dscore
