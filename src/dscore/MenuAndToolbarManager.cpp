#include "MenuAndToolbarManager.h"

#include <spdlog/spdlog.h>

#include "dscore/CoreConstants.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/ICommandProvider.h"
#include "dscore/IMenuProvider.h"
#include "dscore/IToolbarProvider.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

MenuAndToolbarManager::MenuAndToolbarManager(QObject* parent) : QObject(parent) {}

void MenuAndToolbarManager::Build() {  // NOLINT
  SPDLOG_INFO("[MenuAndToolbarManager] Starting UI Build Phase...");

  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  if (command_manager == nullptr) {
    SPDLOG_ERROR("[MenuAndToolbarManager] CommandManager missing. Aborting Build.");
    return;
  }

  // 0. Register Commands FIRST
  // 这是关键的：命令必须在添加到菜单/工具栏之前进行注册。
  SPDLOG_INFO("[MenuAndToolbarManager] Collecting Command Providers...");
  auto command_providers = sss::extsystem::GetTObjects<sss::dscore::ICommandProvider>();
  SPDLOG_INFO("[MenuAndToolbarManager] Found {} Command Providers.", command_providers.size());
  for (auto* provider : command_providers) {
    if (provider != nullptr) {
      auto* qobj = dynamic_cast<QObject*>(provider);
      SPDLOG_DEBUG("[MenuAndToolbarManager] Invoking Command Provider: {}",
                   qobj ? qobj->metaObject()->className() : "Unknown");
      provider->RegisterCommands(command_manager);
    }
  }

  // 1. Create Base Menu Structure (File, View, Help, etc.)
  // 这确保主菜单栏存在。
  auto* app_menu = command_manager->FindContainer(sss::dscore::constants::menubars::kApplication);
  if (app_menu == nullptr) {
    SPDLOG_INFO("[MenuAndToolbarManager] Creating Application Menu Bar root.");
    command_manager->CreateActionContainer(sss::dscore::constants::menubars::kApplication,
                                           sss::dscore::ContainerType::kMenuBar, nullptr, 0);
  }

  // 2. Discover and Invoke Menu Providers
  SPDLOG_INFO("[MenuAndToolbarManager] Collecting Menu Providers...");
  auto menu_providers = sss::extsystem::GetTObjects<sss::dscore::IMenuProvider>();
  SPDLOG_INFO("[MenuAndToolbarManager] Found {} Menu Providers.", menu_providers.size());
  for (auto* provider : menu_providers) {
    if (provider != nullptr) {
      auto* qobj = dynamic_cast<QObject*>(provider);
      SPDLOG_DEBUG("[MenuAndToolbarManager] Invoking Menu Provider: {}",
                   qobj ? qobj->metaObject()->className() : "Unknown");
      provider->ContributeToMenu(command_manager);
    }
  }

  // 3. Discover and Invoke Toolbar Providers
  SPDLOG_INFO("[MenuAndToolbarManager] Collecting Toolbar Providers...");
  auto toolbar_providers = sss::extsystem::GetTObjects<sss::dscore::IToolbarProvider>();
  SPDLOG_INFO("[MenuAndToolbarManager] Found {} Toolbar Providers.", toolbar_providers.size());
  for (auto* provider : toolbar_providers) {
    if (provider != nullptr) {
      auto* qobj = dynamic_cast<QObject*>(provider);
      SPDLOG_DEBUG("[MenuAndToolbarManager] Invoking Toolbar Provider: {}",
                   qobj ? qobj->metaObject()->className() : "Unknown");
      provider->ContributeToToolbar(command_manager);
    }
  }

  // 4. Refresh UI Text (Retranslate) to ensure initial labels are correct
  command_manager->RetranslateUi();

  SPDLOG_INFO("[MenuAndToolbarManager] UI Build Phase Completed.");
}

}  // namespace sss::dscore
