#include "CoreComponent.h"

#include <spdlog/spdlog.h>

#include "CommandManager.h"
#include "ContextManager.h"
#include "Core.h"
#include "MainWindow.h"

CoreComponent::CoreComponent() : core_(nullptr), context_manager_(nullptr), command_manager_(nullptr) {}

CoreComponent::~CoreComponent() = default;

auto CoreComponent::InitialiseEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialiseEvent started");

  core_ = new sss::dscore::Core();
  SPDLOG_INFO("[CoreComponent] Created Core instance: {}", (void*)core_);
  sss::extsystem::AddObject(core_);

  context_manager_ = new sss::dscore::ContextManager();
  SPDLOG_INFO("[CoreComponent] Created ContextManager instance: {}", (void*)context_manager_);
  sss::extsystem::AddObject(context_manager_);

  command_manager_ = new sss::dscore::CommandManager();
  SPDLOG_INFO("[CoreComponent] Created CommandManager instance: {}", (void*)command_manager_);
  sss::extsystem::AddObject(command_manager_);

  auto* main_window = qobject_cast<sss::dscore::MainWindow*>(sss::dscore::ICore::GetInstance()->GetMainWindow());
  SPDLOG_INFO("[CoreComponent] MainWindow obtained: {}", (void*)main_window);

  main_window->Initialise();
  SPDLOG_INFO("[CoreComponent] MainWindow initialised");

  SPDLOG_INFO("[CoreComponent] InitialiseEvent completed");
}

auto CoreComponent::InitialisationFinishedEvent() -> void {
  SPDLOG_INFO("[CoreComponent] InitialisationFinishedEvent started");

  // 直接使用AllObjects避免GetObject宏问题
  sss::dscore::Core* core = nullptr;
  for (auto* object : sss::extsystem::AllObjects()) {
    core = qobject_cast<sss::dscore::Core*>(object);
    if (core != nullptr) {
      break;
    }
  }
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

  delete core_;
  SPDLOG_INFO("[CoreComponent] Core deleted");

  delete context_manager_;
  SPDLOG_INFO("[CoreComponent] ContextManager deleted");

  delete command_manager_;
  SPDLOG_INFO("[CoreComponent] CommandManager deleted");

  SPDLOG_INFO("[CoreComponent] FinaliseEvent completed");
}
