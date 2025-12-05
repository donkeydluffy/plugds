#include "CoreComponent.h"

#include <spdlog/spdlog.h>

#include <QAction>

#include "CommandManager.h"
#include "ContextManager.h"
#include "Core.h"
#include "CoreUIProvider.h"
#include "LanguageService.h"
#include "MainWindow.h"
#include "MenuAndToolbarManager.h"
#include "ThemeService.h"
#include "dscore/IMode.h"
#include "dscore/IModeManager.h"
#include "dscore/IStatusbarManager.h"
#include "dscore/IStatusbarProvider.h"

CoreComponent::CoreComponent() = default;

CoreComponent::~CoreComponent() = default;

auto CoreComponent::InitialiseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialiseEvent started (Phase 1: Infrastructure initialization)");

  // 第一阶段：基础设施初始化
  // 在此阶段，我们建立核心管理器。插件将依赖这些已存在的管理器
  // 当它们的 InitialiseEvent (第二阶段) 被调用时。
  // 不应在此处构建依赖于其他插件的 UI。

  // 1. 首先初始化基础设施服务
  context_manager_ = std::make_unique<sss::dscore::ContextManager>();
  sss::extsystem::AddObject(context_manager_.get());

  command_manager_ = std::make_unique<sss::dscore::CommandManager>();
  sss::extsystem::AddObject(command_manager_.get());

  // 语言和主题服务
  language_service_ = std::make_unique<sss::dscore::LanguageService>();
  sss::extsystem::AddObject(language_service_.get());

  // 注册 dscore 翻译文件
  language_service_->RegisterTranslator("dscore", ":/dscore/i18n");

  theme_service_ = std::make_unique<sss::dscore::ThemeService>();
  sss::extsystem::AddObject(theme_service_.get());

  // 加载默认主题
  theme_service_->LoadTheme("dark");

  // 2. 创建并注册 UI 提供者
  core_ui_provider_ = std::make_unique<sss::dscore::CoreUIProvider>();
  sss::extsystem::AddObject(core_ui_provider_.get());
  SPDLOG_INFO("[CoreComponent] Created and registered CoreUIProvider");

  // 3. 初始化 Core（创建 MainWindow 和 PageManager，依赖于服务）
  core_ = std::make_unique<sss::dscore::Core>();
  sss::extsystem::AddObject(core_.get());

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());

  main_window->Initialise();
  SPDLOG_INFO("[CoreComponent] MainWindow initialized");

  SPDLOG_INFO("[CoreComponent] InitialiseEvent completed");
}

auto CoreComponent::InitialisationFinishedEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent started (Phase 3: UI composition and display)");

  // 第三阶段：UI 组合与展示
  // 此事件按反向加载顺序调用。由于 Core 最先加载，此函数最后执行。
  // 此时，所有其他插件都已完成它们的 InitialiseEvent。
  // 现在可以安全地：
  // 1. 发现来自所有插件的服务提供者（菜单、工具栏、状态栏）。
  // 2. 构建最终的用户界面。
  // 3. 激活默认的模式/上下文。
  // 4. 显示主窗口。

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());
  if (main_window == nullptr) {
    SPDLOG_ERROR("[CoreComponent] Failed to get MainWindow instance in InitialisationFinishedEvent.");
    return;
  }

  // 直接使用拥有的 core_ 成员
  if (core_) {
    // 触发 MenuAndToolbarManager 从已注册的服务提供者构建 UI
    menu_and_toolbar_manager_ = std::make_unique<sss::dscore::MenuAndToolbarManager>();
    menu_and_toolbar_manager_->Build();

    // 处理状态栏服务提供者（第三阶段）
    auto* statusbar_manager = sss::extsystem::GetTObject<sss::dscore::IStatusbarManager>();
    if (statusbar_manager != nullptr) {
      auto statusbar_providers = sss::extsystem::GetTObjects<sss::dscore::IStatusbarProvider>();
      SPDLOG_INFO("[CoreComponent] Found {} statusbar providers.", statusbar_providers.size());
      for (auto* provider : statusbar_providers) {
        provider->ContributeToStatusbar(statusbar_manager);
      }
    }

    // 3. 激活默认模式/上下文
    auto* mode_manager = sss::extsystem::GetTObject<sss::dscore::IModeManager>();
    if (mode_manager != nullptr) {
      auto modes = mode_manager->Modes();
      if (!modes.isEmpty()) {
        // 注意：将来从 ISettingsService 读取"LastMode"。
        // 目前选择第一个（如果加载了，可能是 WS1）。
        auto* first_mode = modes.first();
        SPDLOG_INFO("[CoreComponent] Activating initial mode: {}", first_mode->Title().toStdString());
        mode_manager->ActivateMode(first_mode->Id());
      } else {
        SPDLOG_WARN("[CoreComponent] No modes registered. Application may be empty.");
      }
    } else {
      SPDLOG_ERROR("[CoreComponent] IModeManager not found.");
    }

    core_->Open();
    SPDLOG_INFO("[CoreComponent] Core opened, main window should now be visible");
  } else {
    SPDLOG_ERROR("[CoreComponent] Core instance is null, cannot open.");
  }

  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent completed");
}

auto CoreComponent::FinaliseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] FinaliseEvent started");

  if (core_) {
    sss::extsystem::RemoveObject(core_.get());
    SPDLOG_INFO("[CoreComponent] Core removed");
  }

  if (core_ui_provider_) {
    sss::extsystem::RemoveObject(core_ui_provider_.get());
    SPDLOG_INFO("[CoreComponent] CoreUIProvider removed");
  }

  if (context_manager_) {
    sss::extsystem::RemoveObject(context_manager_.get());
    SPDLOG_INFO("[CoreComponent] ContextManager removed");
  }

  if (command_manager_) {
    sss::extsystem::RemoveObject(command_manager_.get());
    SPDLOG_INFO("[CoreComponent] CommandManager removed");
  }

  if (language_service_) {
    sss::extsystem::RemoveObject(language_service_.get());
    SPDLOG_INFO("[CoreComponent] LanguageService removed");
  }

  if (theme_service_) {
    sss::extsystem::RemoveObject(theme_service_.get());
    SPDLOG_INFO("[CoreComponent] ThemeService removed");
  }

  // unique_ptr 将在此处自动删除

  SPDLOG_INFO("[CoreComponent] FinaliseEvent completed");
}
