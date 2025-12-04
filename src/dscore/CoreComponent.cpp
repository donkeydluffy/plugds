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
  SPDLOG_INFO("[CoreComponent] InitialiseEvent 开始（第一阶段：基础设施初始化）");

  // 第一阶段：基础设施初始化
  // 在此阶段，我们建立核心管理器。插件将依赖这些已存在的管理器
  // 当它们的 InitialiseEvent (第二阶段) 被调用时。
  // 不应在此处构建依赖于其他插件的 UI。

  // 1. 首先初始化基础设施服务
  context_manager_ = std::make_unique<sss::dscore::ContextManager>();
  SPDLOG_INFO("[CoreComponent] 已创建 ContextManager 实例：{}", (void*)context_manager_.get());
  sss::extsystem::AddObject(context_manager_.get());

  command_manager_ = std::make_unique<sss::dscore::CommandManager>();
  SPDLOG_INFO("[CoreComponent] 已创建 CommandManager 实例：{}", (void*)command_manager_.get());
  sss::extsystem::AddObject(command_manager_.get());

  // 语言和主题服务
  language_service_ = std::make_unique<sss::dscore::LanguageService>();
  SPDLOG_INFO("[CoreComponent] 已创建 LanguageService 实例：{}", (void*)language_service_.get());
  sss::extsystem::AddObject(language_service_.get());

  // 注册 dscore 翻译文件
  language_service_->RegisterTranslator("dscore", ":/dscore/i18n");

  theme_service_ = std::make_unique<sss::dscore::ThemeService>();
  SPDLOG_INFO("[CoreComponent] 已创建 ThemeService 实例：{}", (void*)theme_service_.get());
  sss::extsystem::AddObject(theme_service_.get());

  // 加载默认主题
  theme_service_->LoadTheme("dark");

  // 2. 创建并注册 UI 提供者
  core_ui_provider_ = std::make_unique<sss::dscore::CoreUIProvider>();
  sss::extsystem::AddObject(core_ui_provider_.get());
  SPDLOG_INFO("[CoreComponent] 已创建并注册 CoreUIProvider");

  // 3. 初始化 Core（创建 MainWindow 和 PageManager，依赖于服务）
  core_ = std::make_unique<sss::dscore::Core>();
  SPDLOG_INFO("[CoreComponent] 已创建 Core 实例：{}", (void*)core_.get());
  sss::extsystem::AddObject(core_.get());

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());
  SPDLOG_INFO("[CoreComponent] 已获取 MainWindow：{}", (void*)main_window);

  main_window->Initialise();
  SPDLOG_INFO("[CoreComponent] MainWindow 已初始化");

  SPDLOG_INFO("[CoreComponent] InitialiseEvent 已完成");
}

auto CoreComponent::InitialisationFinishedEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent 开始（第三阶段：UI 组合与展示）");

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
    SPDLOG_ERROR("[CoreComponent] 在 InitialisationFinishedEvent 中获取 MainWindow 实例失败。");
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
      SPDLOG_INFO("[CoreComponent] 找到 {} 个状态栏提供者。", statusbar_providers.size());
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
        SPDLOG_INFO("[CoreComponent] 正在激活初始模式：{}", first_mode->Title().toStdString());
        mode_manager->ActivateMode(first_mode->Id());
      } else {
        SPDLOG_WARN("[CoreComponent] 没有注册任何模式。应用程序可能为空。");
      }
    } else {
      SPDLOG_ERROR("[CoreComponent] 找不到 IModeManager。");
    }

    core_->Open();
    SPDLOG_INFO("[CoreComponent] Core 已打开，主窗口现在应该可见");
  } else {
    SPDLOG_ERROR("[CoreComponent] Core 实例为空，无法打开。");
  }

  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent 已完成");
}

auto CoreComponent::FinaliseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] FinaliseEvent 开始");

  if (core_) {
    sss::extsystem::RemoveObject(core_.get());
    SPDLOG_INFO("[CoreComponent] Core 已移除");
  }

  if (core_ui_provider_) {
    sss::extsystem::RemoveObject(core_ui_provider_.get());
    SPDLOG_INFO("[CoreComponent] CoreUIProvider 已移除");
  }

  if (context_manager_) {
    sss::extsystem::RemoveObject(context_manager_.get());
    SPDLOG_INFO("[CoreComponent] ContextManager 已移除");
  }

  if (command_manager_) {
    sss::extsystem::RemoveObject(command_manager_.get());
    SPDLOG_INFO("[CoreComponent] CommandManager 已移除");
  }

  if (language_service_) {
    sss::extsystem::RemoveObject(language_service_.get());
    SPDLOG_INFO("[CoreComponent] LanguageService 已移除");
  }

  if (theme_service_) {
    sss::extsystem::RemoveObject(theme_service_.get());
    SPDLOG_INFO("[CoreComponent] ThemeService 已移除");
  }

  // unique_ptr 将在此处自动删除

  SPDLOG_INFO("[CoreComponent] FinaliseEvent 已完成");
}
