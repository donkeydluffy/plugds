#pragma once

#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>

#include "dscore/IContextManager.h"

namespace sss::dscore {
/**
 * @brief       ContextManager 类用于管理当前应用程序上下文。
 *
 * @details     上下文是应用程序进入的一种状态，例如应用程序可能有多种编辑器类型，
 *              每个编辑器都有自己的上下文ID。当编辑器变为活动状态时，该编辑器的上下文
 *              被设置为活动状态，命令或菜单的可见性或活动状态将被更新。
 */
class ContextManager : public sss::dscore::IContextManager {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IContextManager)

 public:
  /**
   * @brief       构造新的 ContextManager 实例。
   */
  ContextManager();

  /**
   * @brief       向应用程序注册上下文。
   *
   * @details     上下文代表软件进入的一种状态，通常在编辑器获得焦点时。
   *              上下文管理器维护此状态，提供更新菜单以反映当前上下文状态的机制。
   *
   * @see         sss::dscore::IContextManager::registerContext
   *
   * @param[in]   contextIdentifier 新上下文的ID。
   *
   * @returns     此上下文的数字标识符。
   */
  auto RegisterContext(QString context_identifier) -> int override;

  /**
   * @copydoc IContextManager::SetContext
   */
  auto SetContext(int context_identifier) -> int override;
  auto AddActiveContext(int context_id) -> void override;
  auto RemoveActiveContext(int context_id) -> void override;

  /**
   * @copydoc IContextManager::ActivateMode
   */
  auto ActivateMode(int mode_context_id) -> void override;

  /**
   * @copydoc IContextManager::Context
   */
  auto Context() -> int override;

  /**
   * @copydoc IContextManager::getActiveContexts
   */
  [[nodiscard]] auto GetActiveContexts() const -> ContextList override;

  /**
   * @copydoc IContextManager::Context(QString)
   */
  auto Context(QString context_name) -> int override;

 private:
  //! @cond

  QList<int> active_contexts_;
  int next_context_id_;
  QMap<QString, int> context_ids_;

  // Hierarchy Management
  int current_mode_context_id_{0};
  QMap<int, QSet<int>> mode_sub_contexts_storage_;

  //! @endcond
};
}  // namespace sss::dscore
