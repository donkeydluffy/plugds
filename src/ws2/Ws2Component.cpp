#include "Ws2Component.h"

#include <spdlog/spdlog.h>

#include <QAction>
#include <QLocale>
#include <QMessageBox>
#include <QObject>

#include "Ws2Page.h"
#include "Ws2UIProvider.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/IContextManager.h"
#include "dscore/ILanguageService.h"
#include "dscore/IModeManager.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws2 {

void Ws2Component::InitialiseEvent() {
  SPDLOG_INFO("Ws2Component::InitialiseEvent");

  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    SPDLOG_ERROR("Failed to get IContextManager.");
    return;
  }

  // 1. 注册上下文
  page_context_id_ = context_manager->RegisterContext("ws2.context");
  sub_context_id_ = context_manager->RegisterContext("ws2.sub_context.enabled");

  // 2. 创建并注册模式
  ws2_mode_ = new Ws2Page(page_context_id_, this);
  ws2_mode_->SetSubContextId(sub_context_id_);

  auto* mode_manager = sss::extsystem::GetTObject<sss::dscore::IModeManager>();
  if (mode_manager != nullptr) {
    mode_manager->AddMode(ws2_mode_);
  } else {
    SPDLOG_ERROR("Failed to get IModeManager.");
  }

  // 3. 语言和主题服务集成
  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  if (lang_service != nullptr) {
    lang_service->RegisterTranslator("ws2", ":/ws2/i18n");
  } else {
    SPDLOG_WARN("ILanguageService not available.");
  }

  // 4. 创建并注册 UI 提供者
  ui_provider_ = std::make_unique<Ws2UIProvider>(this, page_context_id_, sub_context_id_);
  sss::extsystem::AddObject(ui_provider_.get());

  // 5. 主题连接
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws2Component::UpdateIcons);
    if (theme_service->Theme() != nullptr) {
      UpdateIcons(theme_service->Theme()->Id());
    } else {
      UpdateIcons("light");
    }
  }
}

void Ws2Component::UpdateIcons(const QString& /*theme_id*/) {  // NOLINT
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();

  if ((command_manager == nullptr) || (theme_service == nullptr)) return;

  const QString base_path = ":/ws2/resources/icons";

  auto set_icon = [&](const QString& cmd_id, const QString& filename) {
    auto* cmd = command_manager->FindCommand(cmd_id);
    if (cmd && cmd->Action()) {
      cmd->Action()->setIcon(theme_service->GetIcon(base_path, filename));
    }
  };

  set_icon("ws2.sample_command", "sample.svg");
}

void Ws2Component::InitialisationFinishedEvent() {}

void Ws2Component::FinaliseEvent() {
  SPDLOG_INFO("Ws2Component::FinaliseEvent");

  if (ui_provider_) {
    sss::extsystem::RemoveObject(ui_provider_.get());
  }
  sss::extsystem::RemoveObject(this);
}

}  // namespace sss::ws2
