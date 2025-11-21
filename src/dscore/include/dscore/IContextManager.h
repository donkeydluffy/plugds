#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"
#include "extsystem/IInterface.h"

namespace sss::dscore {
using ContextList = QList<int>;

constexpr int kGlobalContext = 0;

/**
 * @brief       The ContextManager interface is used to manage the current application context.
 *
 * @details     A context is a state that the application enters, for example an application may have multiple
 *              editor types and each editor will have its own context id.  When an editor becomes active, the
 *              context for that editor is set as active, and the visibility or active state of commands or menus
 *              will be updated.
 *
 * @class       sss::dscore::IContextManager IContextManager.h <IContextManager>
 */
class NEDRYSOFT_CORE_DLLSPEC IContextManager : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the IContextManager instance.
   */
  static auto GetInstance() -> IContextManager* { return sss::extsystem::GetObject<IContextManager>(); }

  /**
   * @brief       Registers a context with the application.
   *
   * @details     A context represents a state that the software enters, usually when an editor has
   *              focus.  The context manager maintains this providing a mechanism to update menus
   *              the reflect the current context state.
   *
   * @param[in]   contextIdentifier the id of the new context.
   *
   * @returns     the numeric identifier of this context.
   */
  virtual auto RegisterContext(QString context_identifier) -> int = 0;

  /**
   * @brief       Sets the current context.
   *
   * @details     Sets the currently active context to the given identifier.
   *
   * @param[in]   contextIdentifier the id of the context being activated.
   *
   * @returns     the status of the context switch.
   */
  virtual auto SetContext(int context_identifier) -> int = 0;

  /**
   * @brief       Gets the current context.
   *
   * @details     Gets the numeric identifier of the current context.
   *
   * @returns     the context identifier.
   */
  virtual auto Context() -> int = 0;

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

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IContextManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IContextManager, "com.nedrysoft.core.IContextManager/1.0.0")
