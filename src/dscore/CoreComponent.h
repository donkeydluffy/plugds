#pragma once

#include <memory>

#include "LanguageService.h"
#include "ThemeService.h"
#include "dscore/CoreSpec.h"
#include "extsystem/IComponent.h"

namespace sss::dscore {
class CommandManager;
class ContextManager;
class Core;
class SystemTrayIconManager;
}  // namespace sss::dscore

/**
 * @brief       The Core component
 *
 * @see         sss::extsystem::IComponent
 *
 * @details     The core component provides the SDK base for the application, it provides the core
 *              mechanisms that allow other components to extend the application
 */
class DS_CORE_DLLSPEC CoreComponent : public QObject, public sss::extsystem::IComponent {
 private:
  Q_OBJECT

  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")

  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  /**
   * @brief       Constructs a new CoreComponent instance.
   */
  CoreComponent();

  /**
   * @brief       Destroys the CoreComponent.
   */
  ~CoreComponent() override;

  /**
   * @brief       initialiseEvent
   *
   * @details     Called by the component loader after all components have been loaded, called in load order.
   *
   * @see         sss::extsystem::IComponent::initialiseEvent
   */
  auto InitialiseEvent() -> void override;

  /**
   * @brief       initialisationFinishedEvent
   *
   * @details     Called by the component loader after all components have been initialised, called in reverse
   *              load order.
   *
   * @see         sss::extsystem::IComponent::initialisationFinishedEvent
   */
  auto InitialisationFinishedEvent() -> void override;

  /**
   * @brief       finaliseEvent
   *
   * @details     Called by the component loader to unload the component.
   *
   * @see         sss::extsystem::IComponent::finaliseEvent
   */
  auto FinaliseEvent() -> void override;

 private:
  //! @cond

  std::unique_ptr<sss::dscore::ContextManager> context_manager_;
  std::unique_ptr<sss::dscore::CommandManager> command_manager_;

  // Language and Theme services
  std::unique_ptr<sss::dscore::LanguageService> language_service_;
  std::unique_ptr<sss::dscore::ThemeService> theme_service_;

  std::unique_ptr<sss::dscore::Core> core_;

  //! @endcond
};
