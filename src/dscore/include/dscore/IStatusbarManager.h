#pragma once

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {
/**
 * @brief       IStatusbarManager 描述状态栏管理器。
 *
 * @details     状态栏管理器处理应用程序状态栏的管理，组件可以使用状态栏来显示
 *              正在运行的任务或结果的信息。
 *
 *              状态栏通常是上下文敏感的，虽然组件可以创建固定的条目，
 *              无论当前应用程序上下文如何都会显示
 *
 * @class       sss::dscore::IStatusbarManager IStatusbarManager.h <IStatusbarManager>
 */
class DS_CORE_DLLSPEC IStatusbarManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       返回 IStatusbarManager 实例。
   *
   * @returns     IStatusbarManager 实例。
   */
  static auto GetInstance() -> IStatusbarManager* { return sss::extsystem::GetTObject<IStatusbarManager>(); }

  /**
   * @brief       在状态栏中显示临时消息。
   *
   * @param[in]   message 要显示的消息。
   * @param[in]   timeout 显示消息的时间（毫秒，0 = 无限期）。
   */
  virtual void SetStatusMessage(const QString& message, int timeout) = 0;

  /**
   * @brief       清除当前状态消息。
   */
  virtual void ClearStatusMessage() = 0;

  /**
   * @brief       向状态栏添加永久控件。
   *
   * @param[in]   widget 要添加的控件（所有权转移到状态栏）。
   * @param[in]   stretch 拉伸因子。
   */
  virtual void AddPermanentWidget(QWidget* widget, int stretch) = 0;

  /**
   * @brief       从状态栏移除永久控件。
   *
   * @param[in]   widget 要移除的控件。
   */
  virtual void RemovePermanentWidget(QWidget* widget) = 0;

  // 具有虚函数的类不应有公共的虚析构函数：
  ~IStatusbarManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IStatusbarManager, "sss.dscore.IStatusbarManager/1.0.0")
