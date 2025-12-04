#pragma once

#include <QIcon>
#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief IMode 接口定义一个明确的应用程序状态或工作空间。
 * 插件实现此接口来定义一个主要模式（例如："扫描"、"报告"、"设置"）。
 */
class DS_CORE_DLLSPEC IMode : public QObject {
  Q_OBJECT

 public:
  explicit IMode(QObject* parent = nullptr);
  ~IMode() override = default;

  /**
   * @brief 此模式的唯一标识符。
   */
  [[nodiscard]] virtual QString Id() const = 0;

  /**
   * @brief 模式选择器的显示名称。
   */
  [[nodiscard]] virtual QString Title() const = 0;

  /**
   * @brief 模式选择器的图标。
   */
  [[nodiscard]] virtual QIcon Icon() const = 0;

  /**
   * @brief 与此模式关联的上下文ID。
   * 当此模式处于活动状态时，系统会确保此上下文ID处于活动状态。
   */
  [[nodiscard]] virtual int ContextId() const = 0;

  /**
   * @brief 在模式选择器（UI Shell）中排序模式的优先级。
   * 较低的值 = 在列表中的位置更靠前。
   */
  [[nodiscard]] virtual int Priority() const = 0;

  /**
   * @brief 当模式被激活时调用。
   * 实现应使用全局 IWorkbench 来设置其视图：
   * - SetBackgroundWidget(...)
   * - AddSidePanel(...)
   * - AddSqueezeWidget(...)
   */
  virtual void Activate() = 0;

  /**
   * @brief 当模式被停用时调用。
   * 实现应在必要时进行清理，虽然通常 IWorkbench 会自动处理基于上下文的隐藏。
   * 建议在此处明确清理不应持续存在的控件。
   */
  virtual void Deactivate() = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMode, "sss.dscore.IMode")
