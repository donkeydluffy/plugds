#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {
using ContextList = QList<int>;

constexpr int kGlobalContext = 0;

/**
 * @brief       Enumeration defining the context level in the hierarchy.
 *
 * @details     The context system uses a three-level hierarchy:
 *              - kGlobal: Always active from program start to end
 *              - kWorkspace: Top-level context for each workspace/mode
 *              - kLocal: Sub-context within a workspace
 */
enum class ContextLevel : uint8_t {
  kGlobal = 0,     ///< Global context, always active
  kWorkspace = 1,  ///< Workspace/Mode level context
  kLocal = 2       ///< Local sub-context within workspace
};

/**
 * @brief       The ContextManager interface is used to manage the current application context.
 *
 * @details     A context is a state that the application enters, for example an application may have multiple
 *              editor types and each editor will have its own context id.  When an editor becomes active, the
 *              context for that editor is set as active, and the visibility or active state of commands or menus
 *              will be updated.
 *
 *              The context system uses a three-level hierarchy:
 *              1. GlobalContext: Initialized at startup, always active until program termination
 *              2. Workspace Context: Each workspace/mode has a top-level context that is
 *                 activated/deactivated when switching between workspaces
 *              3. Local Context: Each workspace can have multiple sub-contexts for fine-grained
 *                 control of command visibility and enabled states
 *
 * @class       sss::dscore::IContextManager IContextManager.h <IContextManager>
 */
class DS_CORE_DLLSPEC IContextManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the IContextManager instance.
   */
  static auto GetInstance() -> IContextManager* { return sss::extsystem::GetTObject<IContextManager>(); }

  /**
   * @brief       Registers a context with the application.
   *
   * @details     A context represents a state that the software enters, usually when an editor has
   *              focus.  The context manager maintains this providing a mechanism to update menus
   *              the reflect the current context state.
   *
   * @param[in]   context_identifier the id of the new context.
   *
   * @returns     the numeric identifier of this context.
   */
  virtual auto RegisterContext(QString context_identifier) -> int = 0;

  /**
   * @brief       Registers a context with the application at a specific hierarchy level.
   *
   * @details     Registers a context with an explicit level in the hierarchy. This allows
   *              proper management of context activation/deactivation based on the level.
   *
   * @param[in]   context_identifier the id of the new context.
   * @param[in]   level the hierarchy level of the context.
   *
   * @returns     the numeric identifier of this context.
   */
  virtual auto RegisterContext(QString context_identifier, ContextLevel level) -> int = 0;

  /**
   * @brief       Sets the workspace context, deactivating any previous workspace and its sub-contexts.
   *
   * @details     This activates a workspace-level context and deactivates any previously active
   *              workspace context along with all its sub-contexts. The global context always
   *              remains active.
   *
   * @param[in]   context_identifier the id of the workspace context being activated.
   *
   * @returns     the status of the context switch.
   */
  virtual auto SetContext(int context_identifier) -> int = 0;

  /**
   * @brief       Activates a workspace context.
   *
   * @details     Sets the specified context as the active workspace context. This will
   *              deactivate any previously active workspace and all its sub-contexts.
   *
   * @param[in]   context_id The ID of the workspace context to activate.
   */
  virtual auto ActivateWorkspace(int context_id) -> void = 0;

  /**
   * @brief       Deactivates a workspace context.
   *
   * @details     Deactivates the specified workspace context and all its associated
   *              sub-contexts. Does not affect the global context.
   *
   * @param[in]   context_id The ID of the workspace context to deactivate.
   */
  virtual auto DeactivateWorkspace(int context_id) -> void = 0;

  /**
   * @brief       Adds a context to the list of active contexts.
   *
   * @details     For local/sub-contexts, this adds them to the active list under the
   *              current workspace. They will be automatically deactivated when the
   *              workspace changes.
   *
   * @param[in]   context_id The ID of the context to add.
   */
  virtual auto AddActiveContext(int context_id) -> void = 0;

  /**
   * @brief       Removes a context from the list of active contexts.
   *
   * @param[in]   context_id The ID of the context to remove.
   */
  virtual auto RemoveActiveContext(int context_id) -> void = 0;

  /**
   * @brief       Associates a sub-context with a parent workspace context.
   *
   * @details     Links a local context to its parent workspace. When the workspace
   *              is deactivated, all associated sub-contexts are also deactivated.
   *
   * @param[in]   sub_context_id The sub-context to associate.
   * @param[in]   workspace_context_id The parent workspace context.
   */
  virtual auto AssociateWithWorkspace(int sub_context_id, int workspace_context_id) -> void = 0;

  /**
   * @brief       Gets the current primary context.
   *
   * @details     Gets the numeric identifier of the most recently activated context
   *              (typically the most specific active context).
   *
   * @returns     the context identifier.
   */
  virtual auto Context() -> int = 0;

  /**
   * @brief       Gets the currently active workspace context.
   *
   * @returns     the workspace context identifier, or kGlobalContext if none.
   */
  virtual auto ActiveWorkspace() -> int = 0;

  /**
   * @brief       Gets the complete list of active contexts.
   *
   * @details     Gets the numeric identifiers of all active contexts, including the global
   *              context, active workspace context, and any active sub-contexts.
   *
   * @returns     A list of active context identifiers.
   */
  [[nodiscard]] virtual auto GetActiveContexts() const -> ContextList = 0;

  /**
   * @brief       Gets the hierarchy level of a context.
   *
   * @param[in]   context_id The context identifier.
   *
   * @returns     the ContextLevel of the context.
   */
  virtual auto GetContextLevel(int context_id) -> ContextLevel = 0;

  /**
   * @brief       Gets the context id by name.
   *
   * @details     Gets the numeric identifier of the given context.
   *
   * @returns     the context identifier.
   */
  virtual auto Context(QString context_name) -> int = 0;

  /**
   * @brief       Signals that the context has changed.
   *
   * @details     Emitted when the context manager switches context.
   *
   * @param[in]   newContext the newly activated context.
   * @param[in]   previousContext the previous selected context.
   */
  Q_SIGNAL void ContextChanged(int new_context, int previous_context);

  /**
   * @brief       Signals that the active workspace has changed.
   *
   * @param[in]   new_workspace the newly activated workspace context.
   * @param[in]   previous_workspace the previously active workspace context.
   */
  Q_SIGNAL void WorkspaceChanged(int new_workspace, int previous_workspace);

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IContextManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IContextManager, "sss.dscore.IContextManager/1.0.0")
