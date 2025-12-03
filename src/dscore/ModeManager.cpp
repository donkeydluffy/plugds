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

  // If this is the first mode, activate it? Or wait for explicit activation.
  // Let's wait for explicit activation via UI or config.
}

void ModeManager::ActivateMode(const QString& id) {
  if (!modes_.contains(id)) {
    SPDLOG_WARN("Attempted to activate unknown mode: {}", id.toStdString());
    return;
  }

  IMode* new_mode = modes_[id];
  if (active_mode_ == new_mode) return;

  IMode* old_mode = active_mode_;
  SPDLOG_INFO("Switching Mode: {} -> {}", old_mode ? old_mode->Id().toStdString() : "None", id.toStdString());

  // 1. Deactivate old mode using the new workspace-aware context management
  if (old_mode != nullptr) {
    old_mode->Deactivate();
    // Use DeactivateWorkspace which also clears all sub-contexts
    auto* cm = IContextManager::GetInstance();
    if (cm != nullptr) {
      cm->DeactivateWorkspace(old_mode->ContextId());
    }
  }

  // 2. Update Active Mode Pointer
  active_mode_ = new_mode;

  // 3. Activate new mode
  if (active_mode_ != nullptr) {
    // Use ActivateWorkspace for the new mode's context
    auto* cm = IContextManager::GetInstance();
    if (cm != nullptr) {
      cm->ActivateWorkspace(active_mode_->ContextId());
    }

    // Setup Workbench Content
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
