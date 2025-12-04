#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class IMode;

/**
 * @brief 管理应用程序模式及其激活。
 */
class DS_CORE_DLLSPEC IModeManager : public QObject {
  Q_OBJECT

 public:
  ~IModeManager() override = default;

  /**
   * @brief 向系统注册一个模式。
   * @param mode 要注册的模式实例。
   */
  virtual void AddMode(IMode* mode) = 0;

  /**
   * @brief 通过ID激活特定模式。
   * @param id 模式的唯一标识符。
   */
  virtual void ActivateMode(const QString& id) = 0;

  /**
   * @brief 获取当前活动的模式。
   * @return 指向活动模式的指针，如果没有则为 nullptr。
   */
  [[nodiscard]] virtual IMode* ActiveMode() const = 0;

  /**
   * @brief 获取所有已注册模式的列表。
   */
  [[nodiscard]] virtual QList<IMode*> Modes() const = 0;

 signals:
  /**
   * @brief 当新模式被注册时发出的信号。
   */
  virtual void ModeAdded(IMode* mode) = 0;

  /**
   * @brief 当活动模式更改时发出的信号。
   */
  virtual void ModeChanged(IMode* new_mode, IMode* old_mode) = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IModeManager, "sss.dscore.IModeManager")
