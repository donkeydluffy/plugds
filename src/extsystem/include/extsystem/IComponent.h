#pragma once

#include <QObject>

#include "extsystem/ComponentSystemSpec.h"

#define SsssoftComponentInterfaceIID "com.sss.IComponent/1.0"

namespace sss::extsystem {
/**
 * @brief       The IComponent interface defines the contract for a loadable component.
 *
 * @details     Interface that a component must implement, the plugin system will call various functions to
 *              load and initialise the plugin at the appropriate time.
 *
 * @class       sss::extsystem::IComponent IComponent.h <IComponent>
 */
class COMPONENT_SYSTEM_DLLSPEC IComponent {
 public:
  /**
   * @brief       Destroys the IComponent.
   */
  virtual ~IComponent();

  /**
   * @brief       The initialiseEvent is called by the component loader to initialise the component.
   *
   * @details     Called by the component loader after all components have been loaded, called in load order.
   */
  virtual auto InitialiseEvent() -> void;

  /**
   * @brief       The initialisationFinishedEvent function is called by the component loader after all
   *              components have been initialised.
   *
   * @details     Called by the component loader after all components have been
   *              initialised, called in reverse load order.
   */
  virtual auto InitialisationFinishedEvent() -> void;

  /**
   * @brief       The finaliseEvent method is called before the component is unloaded.
   *
   * @note        The event is called in reverse load order for all loaded components, once every component
   *              has been finalised the component manager then unloads all components in thr same order.
   */
  virtual auto FinaliseEvent() -> void;
};
}  // namespace sss::extsystem

Q_DECLARE_INTERFACE(sss::extsystem::IComponent, SsssoftComponentInterfaceIID)
