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
  SPDLOG_INFO("[CoreComponent] InitialiseEvent started (Phase 1: Infrastructure Initialization)");

  // PHASE 1: Infrastructure Initialization
  // In this phase, we establish the core managers. Plugins will rely on these being present
  // when their InitialiseEvent (Phase 2) is called.
  // NO UI construction should happen here that depends on other plugins.

  // 1. Initialize Infrastructure Services FIRST
  context_manager_ = std::make_unique<sss::dscore::ContextManager>();
  SPDLOG_INFO("[CoreComponent] Created ContextManager instance: {}", (void*)context_manager_.get());
  sss::extsystem::AddObject(context_manager_.get());

  command_manager_ = std::make_unique<sss::dscore::CommandManager>();
  SPDLOG_INFO("[CoreComponent] Created CommandManager instance: {}", (void*)command_manager_.get());
  sss::extsystem::AddObject(command_manager_.get());

  // Language and Theme services
  language_service_ = std::make_unique<sss::dscore::LanguageService>();
  SPDLOG_INFO("[CoreComponent] Created LanguageService instance: {}", (void*)language_service_.get());
  sss::extsystem::AddObject(language_service_.get());

  // Register dscore translations
  language_service_->RegisterTranslator("dscore", ":/dscore/i18n");

  theme_service_ = std::make_unique<sss::dscore::ThemeService>();
  SPDLOG_INFO("[CoreComponent] Created ThemeService instance: {}", (void*)theme_service_.get());
  sss::extsystem::AddObject(theme_service_.get());

  // Load default theme
  theme_service_->LoadTheme("dark");

  // 2. Create and Register UI Provider
  core_ui_provider_ = std::make_unique<sss::dscore::CoreUIProvider>();
  sss::extsystem::AddObject(core_ui_provider_.get());
  SPDLOG_INFO("[CoreComponent] Created and registered CoreUIProvider");

  // 3. Initialize Core (which creates MainWindow and PageManager, dependent on services)
  core_ = std::make_unique<sss::dscore::Core>();
  SPDLOG_INFO("[CoreComponent] Created Core instance: {}", (void*)core_.get());
  sss::extsystem::AddObject(core_.get());

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());
  SPDLOG_INFO("[CoreComponent] MainWindow obtained: {}", (void*)main_window);

  main_window->Initialise();
  SPDLOG_INFO("[CoreComponent] MainWindow initialised");

  SPDLOG_INFO("[CoreComponent] InitialiseEvent completed");
}

auto CoreComponent::InitialisationFinishedEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent started (Phase 3: UI Composition & Presentation)");

  // PHASE 3: UI Composition & Presentation
  // This event is called in REVERSE load order. Since Core is loaded first, this function runs LAST.
  // At this point, all other plugins have finished their InitialiseEvent.
  // It is now safe to:
  // 1. Discover providers (Menu, Toolbar, Statusbar) from all plugins.
  // 2. Build the final UI.
  // 3. Activate the default mode/context.
  // 4. Show the main window.

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());
  if (main_window == nullptr) {
    SPDLOG_ERROR("[CoreComponent] Failed to get MainWindow instance in InitialisationFinishedEvent.");
    return;
  }

  // Use the owned core_ member directly
  if (core_) {
    // Trigger MenuAndToolbarManager to build the UI from registered providers
    menu_and_toolbar_manager_ = std::make_unique<sss::dscore::MenuAndToolbarManager>();
    menu_and_toolbar_manager_->Build();

    // Process Statusbar Providers (Phase 3)
    auto* statusbar_manager = sss::extsystem::GetTObject<sss::dscore::IStatusbarManager>();
    if (statusbar_manager != nullptr) {
      auto statusbar_providers = sss::extsystem::GetTObjects<sss::dscore::IStatusbarProvider>();
      SPDLOG_INFO("[CoreComponent] Found {} Statusbar Providers.", statusbar_providers.size());
      for (auto* provider : statusbar_providers) {
        provider->ContributeToStatusbar(statusbar_manager);
      }
    }

    // 3. Activate the default mode/context
    auto* mode_manager = sss::extsystem::GetTObject<sss::dscore::IModeManager>();
    if (mode_manager != nullptr) {
      auto modes = mode_manager->Modes();
      if (!modes.isEmpty()) {
        // Note: In the future, read "LastMode" from ISettingsService.
        // For now, pick the first one (likely WS1 if loaded).
        auto* first_mode = modes.first();
        SPDLOG_INFO("[CoreComponent] Activating initial mode: {}", first_mode->Title().toStdString());
        mode_manager->ActivateMode(first_mode->Id());
      } else {
        SPDLOG_WARN("[CoreComponent] No modes registered. Application might be empty.");
      }
    } else {
      SPDLOG_ERROR("[CoreComponent] IModeManager not found.");
    }

    core_->Open();
    SPDLOG_INFO("[CoreComponent] Core opened, main window should be visible now");
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

  // unique_ptr will auto-delete here

  SPDLOG_INFO("[CoreComponent] FinaliseEvent completed");
}
