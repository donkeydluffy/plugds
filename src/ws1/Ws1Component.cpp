#include "Ws1Component.h"

#include <spdlog/spdlog.h>

#include <QAction>
#include <QMessageBox>

#include "Ws1Page.h"
#include "dscore/CoreConstants.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommandManager.h"
#include "dscore/IContextManager.h"
#include "dscore/IPageManager.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws1 {

void Ws1Component::InitialiseEvent() {
  SPDLOG_INFO("Ws1Component::InitialiseEvent");

  // 1. Register contexts for this plugin
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager != nullptr) {
    page_context_id_ = context_manager->RegisterContext("ws1.context");
    sub_context_id_ = context_manager->RegisterContext("ws1.sub_context.enabled");
    SPDLOG_INFO("Ws1Component registered page_context_id_: {}, sub_context_id_: {}", page_context_id_, sub_context_id_);
  } else {
    SPDLOG_ERROR("Failed to get IContextManager service.");
    return;
  }

  // 2. Create and register a command with dual-layer contexts
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();
  if (command_manager != nullptr) {
    auto* sample_action = new QAction(tr("Ws1 Sample Command"));
    connect(sample_action, &QAction::triggered, this, []() {
      QMessageBox::information(nullptr, tr("Command Executed"), tr("This is a sample command from the Ws1 plugin."));
    });

    sss::dscore::ContextList visibility_contexts;
    visibility_contexts << page_context_id_;

    sss::dscore::ContextList enabled_contexts;
    enabled_contexts << page_context_id_ << sub_context_id_;

    auto* command =
        command_manager->RegisterAction(sample_action, "ws1.sample_command", visibility_contexts, enabled_contexts);
    SPDLOG_INFO("Ws1Component registered 'ws1.sample_command'");

    // 3. Add the command's action to the main menu and toolbar
    auto* main_menu_bar = command_manager->FindMenu(sss::dscore::constants::menubars::kApplication);
    if (main_menu_bar != nullptr) {
      // For simplicity, adding to the 'File' menu. A dedicated 'Ws1' menu could be created.
      auto* file_menu = command_manager->FindMenu(sss::dscore::constants::menus::kFile);
      if (file_menu != nullptr) {
        file_menu->AppendCommand(command, sss::dscore::constants::menugroups::kTop);
        SPDLOG_INFO("Added 'ws1.sample_command' to File menu.");
      } else {
        SPDLOG_ERROR("Could not find File menu to add command.");
      }
    } else {
      SPDLOG_ERROR("Could not find main menu bar to add command.");
    }

    auto* main_toolbar = command_manager->FindMenu(sss::dscore::constants::toolbars::kMainToolbar);
    if (main_toolbar != nullptr) {
      main_toolbar->AppendCommand(command, sss::dscore::constants::menugroups::kTop);
      SPDLOG_INFO("Added 'ws1.sample_command' to Main toolbar.");
    } else {
      SPDLOG_ERROR("Could not find main toolbar to add command.");
    }

  } else {
    SPDLOG_ERROR("Failed to get ICommandManager service.");
  }

  // 4. Add the page to the UI
  auto* page_manager = sss::extsystem::GetTObject<sss::dscore::IPageManager>();
  if (page_manager != nullptr) {
    page_manager->AddPage(this);
    SPDLOG_INFO("Ws1Page added to PageManager.");
  } else {
    SPDLOG_ERROR("Failed to get IPageManager service.");
  }
}

void Ws1Component::InitialisationFinishedEvent() {
  // Do nothing
}

void Ws1Component::FinaliseEvent() {
  SPDLOG_INFO("Ws1Component::FinaliseEvent");
  auto* page_manager = sss::extsystem::GetTObject<sss::dscore::IPageManager>();
  if (page_manager != nullptr) {
    page_manager->RemovePage(this);
    SPDLOG_INFO("Ws1Page removed from PageManager.");
  } else {
    SPDLOG_ERROR("Failed to get IPageManager service during finalisation.");
  }
}

auto Ws1Component::CreatePage(QWidget* parent) -> QWidget* {
  // Pass the sub_context_id to the page so it can control it
  auto* page = new Ws1Page(parent);
  page->SetSubContextId(sub_context_id_);
  return page;
}

auto Ws1Component::PageTitle() const -> QString { return tr("工作区1"); }

auto Ws1Component::PageContextId() const -> int { return page_context_id_; }

}  // namespace sss::ws1
