#pragma once

#include <QMap>
#include <QObject>

#include "dscore/IModeManager.h"

namespace sss::dscore {

class IWorkbench;

/**
 * @brief Concrete implementation of IModeManager.
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
   * @brief Sets the global workbench instance used for mode content.
   */
  void SetGlobalWorkbench(IWorkbench* workbench);

 private:
  QMap<QString, IMode*> modes_;
  IMode* active_mode_ = nullptr;
  IWorkbench* workbench_ = nullptr;
};

}  // namespace sss::dscore
