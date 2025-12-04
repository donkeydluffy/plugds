#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {
using ContextList = QList<int>;

constexpr int kGlobalContext = 0;

/**
 * @brief       ContextManager 接口用于管理当前应用程序上下文。
 *
 * @details     上下文是应用程序进入的一种状态，例如应用程序可能有多种编辑器类型，
 *              每个编辑器都有自己的上下文ID。当编辑器变为活动状态时，该编辑器的上下文
 *              被设置为活动状态，命令或菜单的可见性或活动状态将被更新。
 *
 * @class       sss::dscore::IContextManager IContextManager.h <IContextManager>
 */
class DS_CORE_DLLSPEC IContextManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       返回 IContextManager 实例。
   */
  static auto GetInstance() -> IContextManager* { return sss::extsystem::GetTObject<IContextManager>(); }

  /**
   * @brief       向应用程序注册上下文。
   *
   * @details     上下文代表软件进入的一种状态，通常在编辑器获得焦点时。
   *              上下文管理器维护此状态，提供更新菜单以反映当前上下文状态的机制。
   *
   * @param[in]   contextIdentifier 新上下文的ID。
   *
   * @returns     此上下文的数字标识符。
   */
  virtual auto RegisterContext(QString context_identifier) -> int = 0;

  /**
   * @brief       设置主页面上下文，清除任何其他活动的子上下文。
   *
   * @details     这将清除所有活动上下文（全局上下文除外），并将页面特定的上下文
   *              设置为给定的标识符。全局上下文始终保持活动状态。
   *
   * @param[in]   contextIdentifier 正在激活的页面上下文的ID。
   *
   * @returns     上下文切换的状态。
   */
  virtual auto SetContext(int context_identifier) -> int = 0;

  /**
   * @brief       向活动上下文列表添加上下文。
   * @param[in]   context_id 要添加的上下文ID。
   */
  virtual auto AddActiveContext(int context_id) -> void = 0;

  /**
   * @brief       从活动上下文列表移除上下文。
   * @param[in]   context_id 要移除的上下文ID。
   */
  virtual auto RemoveActiveContext(int context_id) -> void = 0;

  /**
   * @brief       获取当前页面特定的上下文。
   *
   * @details     获取当前页面特定上下文的数字标识符。
   *
   * @returns     上下文标识符。
   */
  virtual auto Context() -> int = 0;

  /**
   * @brief       获取活动上下文的完整列表。
   *
   * @details     获取所有活动上下文的数字标识符，通常包括全局上下文
   *              和当前页面特定的上下文。
   *
   * @returns     活动上下文标识符列表。
   */
  [[nodiscard]] virtual auto GetActiveContexts() const -> ContextList = 0;

  /**
   * @brief       按名称获取上下文ID。
   *
   * @details     获取给定上下文的数字标识符。
   *
   * @returns     上下文标识符。
   */
  virtual auto Context(QString context_name) -> int = 0;

  /**
   * @brief       激活模式级别的上下文，管理上下文层次结构。
   *
   * @details     切换活动模式上下文。这处理"三级"层次结构：
   *              1. 全局上下文（持续存在）。
   *              2. 模式上下文（切换）。
   *              3. 子上下文（为特定模式恢复）。
   *
   * @param[in]   mode_context_id 要激活模式的上下文ID。
   */
  virtual auto ActivateMode(int mode_context_id) -> void = 0;

  /**
   * @brief       信号表明上下文已更改。
   *
   * @details     当上下文管理器切换上下文时发出。
   *
   * @param[in]   newContext 新激活的上下文。
   * @param[in]   previousContext 之前选择的上下文。
   */
  Q_SIGNAL void ContextChanged(int new_context, int previous_context);

  // 具有虚函数的类不应有公共的虚析构函数：
  ~IContextManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IContextManager, "sss.dscore.IContextManager/1.0.0")
