#include "Ws1Component.h"

#include <spdlog/spdlog.h>

#include <QAction>
#include <QLocale>
#include <QMessageBox>
#include <QObject>

#include "Ws1Page.h"
#include "Ws1UIProvider.h"
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
    // VIOLATION FIX: Do NOT activate mode here (Phase 2).
    // Mode activation involves UI manipulation which should happen in Phase 3 (InitialisationFinishedEvent) or later.
    // mode_manager->ActivateMode(ws1_mode_->Id());
  } else {
    SPDLOG_ERROR("Failed to get IModeManager.");
  }

  // 3. Language & Theme Service Integration
  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  if (lang_service != nullptr) {
    lang_service->RegisterTranslator("ws1", ":/ws1/i18n");
  } else {
    SPDLOG_WARN("ILanguageService not available.");
  }

  // 4. Create and Register UI Provider
  ui_provider_ = std::make_unique<Ws1UIProvider>(this, page_context_id_, sub_context_id_);
  sss::extsystem::AddObject(ui_provider_.get());

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

void Ws1Component::InitialisationFinishedEvent() {}

void Ws1Component::FinaliseEvent() {
  SPDLOG_INFO("Ws1Component::FinaliseEvent");

  if (ui_provider_) {
    sss::extsystem::RemoveObject(ui_provider_.get());
  }
  sss::extsystem::RemoveObject(this);
}

}  // namespace sss::ws1
