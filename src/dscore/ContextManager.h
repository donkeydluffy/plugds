#pragma once

#include <QMap>
#include <QObject>
#include <QString>

#include "dscore/IContextManager.h"

namespace sss::dscore {
/**
 * @brief       The ContextManager class is used to manage the current application context.
 *
 * @details     A context is a state that the application enters, for example an application may have multiple
 *              editor types and each editor will have its own context id.  When an editor becomes active, the
 *              context for that editor is set as active, and the visibility or active state of commands or menus
 *              will be updated.
 */
class ContextManager : public sss::dscore::IContextManager {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IContextManager)

 public:
  /**
   * @brief       Constructs a new ContextManager instance.
   */
  ContextManager();

  /**
   * @brief       Registers a context with the application.
   *
   * @details     A context represents a state that the software enters, usually when an editor has
   *              focus.  The context manager maintains this providing a mechanism to update menus
   *              the reflect the current context state.
   *
   * @see         sss::dscore::IContextManager::registerContext
   *
   * @param[in]   contextIdentifier the id of the new context.
   *
   * @returns     the numeric identifier of this context.
   */
  auto RegisterContext(QString context_identifier) -> int override;

  /**
   * @copydoc IContextManager::SetContext
   */
  auto SetContext(int context_identifier) -> int override;
  auto AddActiveContext(int context_id) -> void override;
  auto RemoveActiveContext(int context_id) -> void override;

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

  //! @endcond
};
}  // namespace sss::dscore
