#pragma once

#include <QMap>
#include <QObject>

#include "dscore/IModeManager.h"

namespace sss::dscore {

class IWorkbench;

/**
 * @brief IModeManager 的具体实现。
 */
class DS_CORE_DLLSPEC ModeManager : public IModeManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IModeManager)

 public:
  explicit ModeManager(QObject* parent = nullptr);
  ~ModeManager() override;

  // IModeManager interface
  void AddMode(IMode* mode) override;
  void ActivateMode(const QString& id) override;
  [[nodiscard]] IMode* ActiveMode() const override;
  [[nodiscard]] QList<IMode*> Modes() const override;

 signals:
  void ModeAdded(IMode* mode) override;
  void ModeChanged(IMode* new_mode, IMode* old_mode) override;

 public:
  /**
   * @brief 设置用于模式内容的全局工作台实例。
   */
  void SetGlobalWorkbench(IWorkbench* workbench);

 private:
  QMap<QString, IMode*> modes_;
  IMode* active_mode_ = nullptr;
  IWorkbench* workbench_ = nullptr;
};

}  // namespace sss::dscore
