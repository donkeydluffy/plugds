#include "CoreComponent.h"

#include <spdlog/spdlog.h>

#include "CommandManager.h"
#include "ContextManager.h"
#include "Core.h"
#include "LanguageService.h"
#include "MainWindow.h"
#include "ThemeService.h"

CoreComponent::CoreComponent() = default;

CoreComponent::~CoreComponent() = default;

auto CoreComponent::InitialiseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialiseEvent started");

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

  // 2. Initialize Core (which creates MainWindow and PageManager, dependent on services)
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
  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent started");

  auto* core = sss::extsystem::GetTObject<sss::dscore::Core>();
  SPDLOG_INFO("[CoreComponent] Got Core instance for open: {}", (void*)core);

  connect(sss::dscore::IContextManager::GetInstance(), &sss::dscore::IContextManager::ContextChanged,
          [&](int new_context, int old_context) {
            Q_UNUSED(old_context)

            sss::dscore::ICommandManager::GetInstance()->SetContext(new_context);
          });

  core->Open();
  SPDLOG_INFO("[CoreComponent] Core opened, main window should be visible now");

  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent completed");
}

auto CoreComponent::FinaliseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] FinaliseEvent started");

  if (core_) {
    sss::extsystem::RemoveObject(core_.get());
    SPDLOG_INFO("[CoreComponent] Core removed");
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
