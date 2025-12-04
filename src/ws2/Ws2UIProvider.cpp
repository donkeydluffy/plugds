#include "Ws2UIProvider.h"

#include <QAction>
#include <QMessageBox>

#include "Ws2Component.h"
#include "dscore/CoreConstants.h"
#include "dscore/CoreStrings.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/IContextManager.h"
#include "dscore/IThemeService.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws2 {

Ws2UIProvider::Ws2UIProvider(Ws2Component* component, int page_context_id, int sub_context_id)
    : QObject(component), component_(component), page_context_id_(page_context_id), sub_context_id_(sub_context_id) {}

void Ws2UIProvider::RegisterCommands(sss::dscore::ICommandManager* command_manager) {
  auto* sample_action = new QAction(tr("Ws2 Sample Command"));

  connect(sample_action, &QAction::triggered, component_, []() {
    QMessageBox::information(nullptr, sss::dscore::CoreStrings::Information(),
                             tr("This is a sample command from the Ws2 plugin."));
  });

  sss::dscore::ContextList visibility_contexts;
  visibility_contexts << page_context_id_;

  sss::dscore::ContextList enabled_contexts;
  enabled_contexts << page_context_id_ << sub_context_id_;

  command_manager->RegisterAction(sample_action, "ws2.sample_command", visibility_contexts, enabled_contexts);

  // 如果主题服务可用，进行初始图标更新
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    const QString base_path = ":/ws2/resources/icons";
    sample_action->setIcon(theme_service->GetIcon(base_path, "sample.svg"));
  }
}

void Ws2UIProvider::ContributeToMenu(sss::dscore::ICommandManager* command_manager) {
  auto* main_menu_bar = command_manager->FindContainer(sss::dscore::constants::menubars::kApplication);
  if (main_menu_bar != nullptr) {
    // 定位在 Ws1（500）和帮助（900）之间。设置为 600。
    auto* ws2_menu =
        command_manager->CreateActionContainer("Ws2", sss::dscore::ContainerType::kMenu, main_menu_bar, 600);
    if (ws2_menu != nullptr) {
      ws2_menu->InsertGroup("Ws2.MainGroup", 0);
      ws2_menu->AppendCommand("ws2.sample_command", "Ws2.MainGroup");
    }
  }
}

void Ws2UIProvider::ContributeToToolbar(sss::dscore::ICommandManager* command_manager) {
  // 为 WS2 创建单独的工具栏，还是添加到现有的？让我们创建一个新的。
  auto* ws2_toolbar =
      command_manager->CreateActionContainer("Ws2Toolbar", sss::dscore::ContainerType::kToolBar, nullptr, 210);
  if (ws2_toolbar != nullptr) {
    ws2_toolbar->InsertGroup("Ws2.ToolbarGroup", 0);
    ws2_toolbar->AppendCommand("ws2.sample_command", "Ws2.ToolbarGroup");
  }
}

}  // namespace sss::ws2
