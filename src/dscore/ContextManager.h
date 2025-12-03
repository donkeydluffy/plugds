#pragma once

#include <QMap>
#include <QMultiMap>
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
 *
 *              This implementation supports a three-level hierarchy:
 *              1. GlobalContext: Always active (kGlobalContext = 0)
 *              2. Workspace Context: Top-level context per workspace/mode
 *              3. Local Context: Sub-contexts within a workspace
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
   * @see         sss::dscore::IContextManager::RegisterContext
   *
   * @param[in]   context_identifier the id of the new context.
   *
   * @returns     the numeric identifier of this context.
   */
  auto RegisterContext(QString context_identifier) -> int override;

  /**
   * @brief       Registers a context with a specific hierarchy level.
   *
   * @param[in]   context_identifier the id of the new context.
   * @param[in]   level the hierarchy level.
   *
   * @returns     the numeric identifier of this context.
   */
  auto RegisterContext(QString context_identifier, ContextLevel level) -> int override;

  /**
   * @copydoc IContextManager::SetContext
   */
  auto SetContext(int context_identifier) -> int override;

  /**
   * @copydoc IContextManager::ActivateWorkspace
   */
  auto ActivateWorkspace(int context_id) -> void override;

  /**
   * @copydoc IContextManager::DeactivateWorkspace
   */
  auto DeactivateWorkspace(int context_id) -> void override;

  auto AddActiveContext(int context_id) -> void override;
  auto RemoveActiveContext(int context_id) -> void override;

  /**
   * @copydoc IContextManager::AssociateWithWorkspace
   */
  auto AssociateWithWorkspace(int sub_context_id, int workspace_context_id) -> void override;

  /**
   * @copydoc IContextManager::Context
   */
  auto Context() -> int override;

  /**
   * @copydoc IContextManager::ActiveWorkspace
   */
  auto ActiveWorkspace() -> int override;

  /**
   * @copydoc IContextManager::GetActiveContexts
   */
  [[nodiscard]] auto GetActiveContexts() const -> ContextList override;

  /**
   * @copydoc IContextManager::GetContextLevel
   */
  auto GetContextLevel(int context_id) -> ContextLevel override;

  /**
   * @copydoc IContextManager::Context(QString)
   */
  auto Context(QString context_name) -> int override;

 private:
  //! @cond

  QList<int> active_contexts_;                   ///< List of all currently active contexts
  int active_workspace_ = kGlobalContext;        ///< Currently active workspace context
  int next_context_id_;                          ///< Next available context ID
  QMap<QString, int> context_ids_;               ///< Map from context name to ID
  QMap<int, ContextLevel> context_levels_;       ///< Map from context ID to hierarchy level
  QMultiMap<int, int> workspace_subcontexts_;    ///< Map from workspace to its sub-contexts

  //! @endcond
};
}  // namespace sss::dscore
