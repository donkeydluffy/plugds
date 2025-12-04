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
class CoreUIProvider;
class MenuAndToolbarManager;
class SystemTrayIconManager;
}  // namespace sss::dscore

/**
 * @brief       核心组件
 *
 * @see         sss::extsystem::IComponent
 *
 * @details     核心组件为应用程序提供 SDK 基础，它提供允许其他组件扩展应用程序的
 *              核心机制
 */
class DS_CORE_DLLSPEC CoreComponent : public QObject, public sss::extsystem::IComponent {
 private:
  Q_OBJECT

  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")

  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  /**
   * @brief       构造新的 CoreComponent 实例。
   */
  CoreComponent();

  /**
   * @brief       销毁 CoreComponent。
   */
  ~CoreComponent() override;

  /**
   * @brief       初始化事件
   *
   * @details     在所有组件加载完成后由组件加载器调用，按加载顺序调用。
   *
   * @see         sss::extsystem::IComponent::initialiseEvent
   */
  auto InitialiseEvent() -> void override;

  /**
   * @brief       初始化完成事件
   *
   * @details     在所有组件初始化完成后由组件加载器调用，按加载顺序的逆序调用。
   *
   * @see         sss::extsystem::IComponent::initialisationFinishedEvent
   */
  auto InitialisationFinishedEvent() -> void override;

  /**
   * @brief       结束事件
   *
   * @details     由组件加载器调用来卸载组件。
   *
   * @see         sss::extsystem::IComponent::finaliseEvent
   */
  auto FinaliseEvent() -> void override;

 private:
  //! @cond

  std::unique_ptr<sss::dscore::ContextManager> context_manager_;
  std::unique_ptr<sss::dscore::CommandManager> command_manager_;

  // 语言和主题服务
  std::unique_ptr<sss::dscore::LanguageService> language_service_;
  std::unique_ptr<sss::dscore::ThemeService> theme_service_;

  std::unique_ptr<sss::dscore::Core> core_;

  // UI 提供器（组合）
  std::unique_ptr<sss::dscore::CoreUIProvider> core_ui_provider_;
  std::unique_ptr<sss::dscore::MenuAndToolbarManager> menu_and_toolbar_manager_;

  //! @endcond
};
