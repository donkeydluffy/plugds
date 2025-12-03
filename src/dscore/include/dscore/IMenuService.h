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

#pragma once

#include <QIcon>
#include <QObject>
#include <QString>
#include <functional>

#include "dscore/CoreSpec.h"
#include "dscore/IActionContainer.h"
#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

/**
 * @brief       Menu item descriptor for registration.
 *
 * @details     Describes a menu item that can be registered with the menu service.
 */
struct MenuItemDescriptor {
  QString command_id;               ///< Unique command identifier
  QString text;                     ///< Display text for the menu item
  QString group_id;                 ///< Group within the parent menu
  QIcon icon;                       ///< Optional icon
  int order = 0;                    ///< Order within the group (lower = earlier)
  ContextList visibility_contexts;  ///< Contexts where this item is visible
  ContextList enabled_contexts;     ///< Contexts where this item is enabled
  std::function<void()> callback;   ///< Optional callback when triggered
};

/**
 * @brief       Menu descriptor for registration.
 *
 * @details     Describes a menu that can be registered with the menu service.
 */
struct MenuDescriptor {
  QString menu_id;                ///< Unique menu identifier
  QString parent_id;              ///< Parent menu/menubar identifier (empty for top-level)
  QString text;                   ///< Display text for the menu
  int order = 0;                  ///< Order within the parent (lower = earlier)
  QList<QString> default_groups;  ///< Default groups to create in this menu
};

/**
 * @brief       The IMenuService interface provides a pluggable menu registration system.
 *
 * @details     Plugins can use this service to register menus and menu items without
 *              directly modifying MainWindow. This enables a decoupled, extensible
 *              menu architecture.
 *
 * @class       sss::dscore::IMenuService IMenuService.h <dscore/IMenuService.h>
 */
class DS_CORE_DLLSPEC IMenuService : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the IMenuService instance.
   */
  static auto GetInstance() -> IMenuService* { return sss::extsystem::GetTObject<IMenuService>(); }

  /**
   * @brief       Registers a new menu.
   *
   * @details     Creates a new menu with the given descriptor. If the menu already exists,
   *              returns the existing menu container.
   *
   * @param[in]   descriptor The menu descriptor containing id, parent, text, and order.
   *
   * @returns     The created or existing IActionContainer for the menu.
   */
  virtual auto RegisterMenu(const MenuDescriptor& descriptor) -> IActionContainer* = 0;

  /**
   * @brief       Registers a menu item (command) to a menu.
   *
   * @details     Creates a command and adds it to the specified menu. The command's
   *              visibility and enabled state are controlled by the specified contexts.
   *
   * @param[in]   menu_id The parent menu identifier.
   * @param[in]   descriptor The menu item descriptor.
   *
   * @returns     The created ICommand, or nullptr on failure.
   */
  virtual auto RegisterMenuItem(const QString& menu_id, const MenuItemDescriptor& descriptor) -> ICommand* = 0;

  /**
   * @brief       Registers an action to an existing command.
   *
   * @details     Allows plugins to provide context-specific implementations of existing
   *              commands (e.g., Open, Save) by registering their own QAction.
   *
   * @param[in]   command_id The existing command identifier.
   * @param[in]   action The action to register.
   * @param[in]   visibility_contexts Contexts where this action is visible.
   * @param[in]   enabled_contexts Contexts where this action is enabled.
   *
   * @returns     true on success, false if command doesn't exist.
   */
  virtual auto RegisterActionForCommand(const QString& command_id, QAction* action,
                                         const ContextList& visibility_contexts,
                                         const ContextList& enabled_contexts) -> bool = 0;

  /**
   * @brief       Inserts a group into a menu.
   *
   * @details     Groups organize menu items and can have separators between them.
   *
   * @param[in]   menu_id The menu identifier.
   * @param[in]   group_id The group identifier.
   * @param[in]   order The display order (lower = earlier).
   */
  virtual auto InsertMenuGroup(const QString& menu_id, const QString& group_id, int order) -> void = 0;

  /**
   * @brief       Finds a menu by its identifier.
   *
   * @param[in]   menu_id The menu identifier.
   *
   * @returns     The IActionContainer if found, nullptr otherwise.
   */
  virtual auto FindMenu(const QString& menu_id) -> IActionContainer* = 0;

  /**
   * @brief       Finds a command by its identifier.
   *
   * @param[in]   command_id The command identifier.
   *
   * @returns     The ICommand if found, nullptr otherwise.
   */
  virtual auto FindCommand(const QString& command_id) -> ICommand* = 0;

  /**
   * @brief       Registers a toolbar.
   *
   * @param[in]   toolbar_id Unique toolbar identifier.
   * @param[in]   order The display order.
   *
   * @returns     The created IActionContainer.
   */
  virtual auto RegisterToolbar(const QString& toolbar_id, int order = 0) -> IActionContainer* = 0;

  /**
   * @brief       Adds a command to a toolbar.
   *
   * @param[in]   toolbar_id The toolbar identifier.
   * @param[in]   command_id The command identifier.
   * @param[in]   group_id The group within the toolbar.
   */
  virtual auto AddToolbarCommand(const QString& toolbar_id, const QString& command_id, const QString& group_id) -> void = 0;

  /**
   * @brief       Signal emitted when a new menu is registered.
   *
   * @param[in]   menu_id The identifier of the newly registered menu.
   */
  Q_SIGNAL void MenuRegistered(const QString& menu_id);

  /**
   * @brief       Signal emitted when a new command is registered.
   *
   * @param[in]   command_id The identifier of the newly registered command.
   */
  Q_SIGNAL void CommandRegistered(const QString& command_id);

  ~IMenuService() override = default;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMenuService, "sss.dscore.IMenuService/1.0.0")
