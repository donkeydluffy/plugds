#include "Ws1Component.h"

#include <spdlog/spdlog.h>

#include <QAction>
#include <QLocale>
#include <QMessageBox>
#include <QObject>

#include "Ws1Page.h"
#include "dscore/CoreConstants.h"
#include "dscore/CoreStrings.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/IContextManager.h"
#include "dscore/ILanguageService.h"
#include "dscore/IModeManager.h"  // Include IModeManager
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws1 {

void Ws1Component::InitialiseEvent() {
  SPDLOG_INFO("Ws1Component::InitialiseEvent");

  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    SPDLOG_ERROR("Failed to get IContextManager.");
    return;
  }

  // 1. Register Contexts
  page_context_id_ = context_manager->RegisterContext("ws1.context");
  sub_context_id_ = context_manager->RegisterContext("ws1.sub_context.enabled");

  // 2. Create and Register Mode
  ws1_mode_ = new Ws1Page(page_context_id_, this);  // Parent is Ws1Component
  ws1_mode_->SetSubContextId(sub_context_id_);      // Fix: Set the sub-context ID!

  auto* mode_manager = sss::extsystem::GetTObject<sss::dscore::IModeManager>();
  if (mode_manager != nullptr) {
    mode_manager->AddMode(ws1_mode_);
    mode_manager->ActivateMode(ws1_mode_->Id());  // Auto-activate for testing
  } else {
    SPDLOG_ERROR("Failed to get IModeManager. Mode will not be activated.");
  }

  // 3. Language & Theme Service Integration
  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  if (lang_service != nullptr) {
    lang_service->RegisterTranslator("ws1", ":/ws1/i18n");
  } else {
    SPDLOG_WARN("ILanguageService not available.");
  }

  // 4. Create Commands
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager != nullptr) {
    createSampleCommand(command_manager);
    // createSwitchCommands removed as they are now in dscore
  }

  // 5. Theme connection
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws1Component::UpdateIcons);
    if (theme_service->Theme() != nullptr) {
      UpdateIcons(theme_service->Theme()->Id());
    } else {
      UpdateIcons("light");
    }
  }
}

void Ws1Component::UpdateIcons(const QString& /*theme_id*/) {  // NOLINT
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();

  if ((command_manager == nullptr) || (theme_service == nullptr)) return;

  const QString base_path = ":/ws1/resources/icons";

  auto set_icon = [&](const QString& cmd_id, const QString& filename) {
    auto* cmd = command_manager->FindCommand(cmd_id);
    if (cmd && cmd->Action()) {
      cmd->Action()->setIcon(theme_service->GetIcon(base_path, filename));
    }
  };

  set_icon("ws1.sample_command", "sample.svg");
}

void Ws1Component::createSampleCommand(sss::dscore::ICommandManager* command_manager) {
  auto* sample_action = new QAction(tr("Ws1 Sample Command"));
  // Icon will be set by updateIcons logic
  connect(sample_action, &QAction::triggered, this, []() {
    QMessageBox::information(nullptr, sss::dscore::CoreStrings::Information(),
                             tr("This is a sample command from the Ws1 plugin."));
  });

  sss::dscore::ContextList visibility_contexts;
  visibility_contexts << page_context_id_;

  sss::dscore::ContextList enabled_contexts;
  enabled_contexts << page_context_id_ << sub_context_id_;

  auto* command =
      command_manager->RegisterAction(sample_action, "ws1.sample_command", visibility_contexts, enabled_contexts);

  // Add to UI - Menu
  auto* main_menu_bar = command_manager->FindContainer(sss::dscore::constants::menubars::kApplication);
  if (main_menu_bar != nullptr) {
    // Position between Settings (100) and Help (900). Set to 500.
    auto* ws1_menu =
        command_manager->CreateActionContainer("Ws1", sss::dscore::ContainerType::kMenu, main_menu_bar, 500);
    ws1_menu->InsertGroup("Ws1.MainGroup", 0);
    ws1_menu->AppendCommand(command, "Ws1.MainGroup");
  }

  // Add to Toolbar
  // We can add to the main toolbar or create a new one.
  // Requirement: "ws1工具栏保留SampleCommand". If we create a new toolbar, we might have two toolbars.
  // Let's append to MainToolbar for cleaner look, OR create Ws1Toolbar.
  // Previous code created Ws1Toolbar. Let's stick to that but ensure it's clean.
  auto* ws1_toolbar =
      command_manager->CreateActionContainer("Ws1Toolbar", sss::dscore::ContainerType::kToolBar, nullptr, 200);
  ws1_toolbar->InsertGroup("Ws1.ToolbarGroup", 0);
  ws1_toolbar->AppendCommand(command, "Ws1.ToolbarGroup");
}

// createSwitchCommands removed

void Ws1Component::InitialisationFinishedEvent() {}

void Ws1Component::FinaliseEvent() {
  SPDLOG_INFO("Ws1Component::FinaliseEvent");
  sss::extsystem::RemoveObject(this);
}

}  // namespace sss::ws1
