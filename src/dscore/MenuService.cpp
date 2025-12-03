/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 27/03/2020.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MenuService.h"

#include <spdlog/spdlog.h>

#include <QAction>

#include "dscore/CoreConstants.h"
#include "dscore/ICommandManager.h"

namespace sss::dscore {

MenuService::MenuService(QObject* parent) : IMenuService() { setParent(parent); }

MenuService::~MenuService() = default;

auto MenuService::RegisterMenu(const MenuDescriptor& descriptor) -> IActionContainer* {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::RegisterMenu: CommandManager not available");
    return nullptr;
  }

  // Check if menu already exists
  auto* existing_menu = command_manager->FindContainer(descriptor.menu_id);
  if (existing_menu != nullptr) {
    SPDLOG_INFO("MenuService::RegisterMenu: Menu '{}' already exists", descriptor.menu_id.toStdString());
    return existing_menu;
  }

  // Find parent container
  IActionContainer* parent_container = nullptr;
  if (!descriptor.parent_id.isEmpty()) {
    parent_container = command_manager->FindContainer(descriptor.parent_id);
    if (parent_container == nullptr) {
      SPDLOG_WARN("MenuService::RegisterMenu: Parent '{}' not found for menu '{}'", descriptor.parent_id.toStdString(),
                  descriptor.menu_id.toStdString());
    }
  }

  // Create the menu
  auto* menu =
      command_manager->CreateActionContainer(descriptor.menu_id, ContainerType::kMenu, parent_container, descriptor.order);

  if (menu != nullptr) {
    // Create default groups
    for (const auto& group_id : descriptor.default_groups) {
      menu->InsertGroup(group_id, 0);
    }

    // Store descriptor for reference
    registered_menus_[descriptor.menu_id] = descriptor;

    SPDLOG_INFO("MenuService::RegisterMenu: Registered menu '{}' (parent='{}')", descriptor.menu_id.toStdString(),
                descriptor.parent_id.toStdString());

    Q_EMIT MenuRegistered(descriptor.menu_id);
  }

  return menu;
}

auto MenuService::RegisterMenuItem(const QString& menu_id, const MenuItemDescriptor& descriptor) -> ICommand* {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::RegisterMenuItem: CommandManager not available");
    return nullptr;
  }

  // Find the target menu
  auto* menu = command_manager->FindContainer(menu_id);
  if (menu == nullptr) {
    SPDLOG_WARN("MenuService::RegisterMenuItem: Menu '{}' not found", menu_id.toStdString());
    return nullptr;
  }

  // Create the action
  auto* action = new QAction(descriptor.text);
  if (!descriptor.icon.isNull()) {
    action->setIcon(descriptor.icon);
  }

  // Connect callback if provided
  if (descriptor.callback) {
    QObject::connect(action, &QAction::triggered, descriptor.callback);
  }

  // Register the action with the command manager
  auto* command =
      command_manager->RegisterAction(action, descriptor.command_id, descriptor.visibility_contexts, descriptor.enabled_contexts);

  if (command != nullptr) {
    // Add to the menu
    QString group_id = descriptor.group_id;
    if (group_id.isEmpty()) {
      group_id = "Ds.Group.Default";
    }
    menu->AppendCommand(command, group_id);

    // Store descriptor for reference
    registered_items_[descriptor.command_id] = descriptor;

    SPDLOG_INFO("MenuService::RegisterMenuItem: Registered item '{}' in menu '{}' (group='{}')",
                descriptor.command_id.toStdString(), menu_id.toStdString(), group_id.toStdString());

    Q_EMIT CommandRegistered(descriptor.command_id);
  }

  return command;
}

auto MenuService::RegisterActionForCommand(const QString& command_id, QAction* action,
                                           const ContextList& visibility_contexts,
                                           const ContextList& enabled_contexts) -> bool {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::RegisterActionForCommand: CommandManager not available");
    return false;
  }

  auto* command = command_manager->FindCommand(command_id);
  if (command == nullptr) {
    SPDLOG_WARN("MenuService::RegisterActionForCommand: Command '{}' not found", command_id.toStdString());
    return false;
  }

  return command_manager->RegisterAction(action, command, visibility_contexts, enabled_contexts);
}

auto MenuService::InsertMenuGroup(const QString& menu_id, const QString& group_id, int order) -> void {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::InsertMenuGroup: CommandManager not available");
    return;
  }

  auto* menu = command_manager->FindContainer(menu_id);
  if (menu == nullptr) {
    SPDLOG_WARN("MenuService::InsertMenuGroup: Menu '{}' not found", menu_id.toStdString());
    return;
  }

  menu->InsertGroup(group_id, order);
  SPDLOG_INFO("MenuService::InsertMenuGroup: Inserted group '{}' in menu '{}'", group_id.toStdString(),
              menu_id.toStdString());
}

auto MenuService::FindMenu(const QString& menu_id) -> IActionContainer* {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    return nullptr;
  }
  return command_manager->FindContainer(menu_id);
}

auto MenuService::FindCommand(const QString& command_id) -> ICommand* {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    return nullptr;
  }
  return command_manager->FindCommand(command_id);
}

auto MenuService::RegisterToolbar(const QString& toolbar_id, int order) -> IActionContainer* {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::RegisterToolbar: CommandManager not available");
    return nullptr;
  }

  // Check if toolbar already exists
  auto* existing_toolbar = command_manager->FindContainer(toolbar_id);
  if (existing_toolbar != nullptr) {
    return existing_toolbar;
  }

  auto* toolbar = command_manager->CreateActionContainer(toolbar_id, ContainerType::kToolBar, nullptr, order);
  if (toolbar != nullptr) {
    SPDLOG_INFO("MenuService::RegisterToolbar: Registered toolbar '{}'", toolbar_id.toStdString());
  }
  return toolbar;
}

auto MenuService::AddToolbarCommand(const QString& toolbar_id, const QString& command_id, const QString& group_id) -> void {
  auto* command_manager = ICommandManager::GetInstance();
  if (command_manager == nullptr) {
    SPDLOG_ERROR("MenuService::AddToolbarCommand: CommandManager not available");
    return;
  }

  auto* toolbar = command_manager->FindContainer(toolbar_id);
  if (toolbar == nullptr) {
    SPDLOG_WARN("MenuService::AddToolbarCommand: Toolbar '{}' not found", toolbar_id.toStdString());
    return;
  }

  auto* command = command_manager->FindCommand(command_id);
  if (command == nullptr) {
    SPDLOG_WARN("MenuService::AddToolbarCommand: Command '{}' not found", command_id.toStdString());
    return;
  }

  toolbar->AppendCommand(command, group_id);
}

}  // namespace sss::dscore
