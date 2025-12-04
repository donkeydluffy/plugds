#include "Ws1UIProvider.h"

#include <QAction>
#include <QMessageBox>

#include "Ws1Component.h"
#include "dscore/CoreConstants.h"
#include "dscore/CoreStrings.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/IContextManager.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"
#include "ws1/Ws1Strings.h"

namespace sss::ws1 {

Ws1UIProvider::Ws1UIProvider(Ws1Component* component, int page_context_id, int sub_context_id)
    : QObject(component), component_(component), page_context_id_(page_context_id), sub_context_id_(sub_context_id) {}

void Ws1UIProvider::RegisterCommands(sss::dscore::ICommandManager* command_manager) {
  auto* sample_action = new QAction(Ws1Strings::SampleCommand());

  connect(sample_action, &QAction::triggered, component_, []() {
    QMessageBox::information(nullptr, sss::dscore::CoreStrings::Information(), Ws1Strings::SampleCommandDesc());
  });

  sss::dscore::ContextList visibility_contexts;
  visibility_contexts << page_context_id_;

  sss::dscore::ContextList enabled_contexts;
  enabled_contexts << page_context_id_ << sub_context_id_;

  command_manager->RegisterAction(sample_action, "ws1.sample_command", visibility_contexts, enabled_contexts);

  // 如果主题服务可用，进行初始图标更新
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    const QString base_path = ":/ws1/resources/icons";
    sample_action->setIcon(theme_service->GetIcon(base_path, "sample.svg"));
  }
}

void Ws1UIProvider::ContributeToMenu(sss::dscore::ICommandManager* command_manager) {
  auto* main_menu_bar = command_manager->FindContainer(sss::dscore::constants::menubars::kApplication);
  if (main_menu_bar != nullptr) {
    // 定位在设置（100）和帮助（900）之间。设置为 500。
    auto* ws1_menu =
        command_manager->CreateActionContainer("Ws1", sss::dscore::ContainerType::kMenu, main_menu_bar, 500);
    if (ws1_menu != nullptr) {
      ws1_menu->InsertGroup("Ws1.MainGroup", 0);
      ws1_menu->AppendCommand("ws1.sample_command", "Ws1.MainGroup");
    }
  }
}

void Ws1UIProvider::ContributeToToolbar(sss::dscore::ICommandManager* command_manager) {
  auto* ws1_toolbar =
      command_manager->CreateActionContainer("Ws1Toolbar", sss::dscore::ContainerType::kToolBar, nullptr, 200);
  if (ws1_toolbar != nullptr) {
    ws1_toolbar->InsertGroup("Ws1.ToolbarGroup", 0);
    ws1_toolbar->AppendCommand("ws1.sample_command", "Ws1.ToolbarGroup");
  }
}

}  // namespace sss::ws1
