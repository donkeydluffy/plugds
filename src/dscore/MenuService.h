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

#include <QMap>
#include <QObject>

#include "dscore/IMenuService.h"

namespace sss::dscore {

class ICommandManager;

/**
 * @brief       The MenuService class provides a pluggable menu registration system.
 *
 * @details     This implementation delegates to ICommandManager for the actual menu/command
 *              creation but provides a simpler, more declarative API for plugins.
 */
class MenuService : public sss::dscore::IMenuService {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IMenuService)

 public:
  /**
   * @brief       Constructs a new MenuService instance.
   */
  explicit MenuService(QObject* parent = nullptr);

  /**
   * @brief       Destroys the MenuService.
   */
  ~MenuService() override;

  // IMenuService interface
  auto RegisterMenu(const MenuDescriptor& descriptor) -> IActionContainer* override;
  auto RegisterMenuItem(const QString& menu_id, const MenuItemDescriptor& descriptor) -> ICommand* override;
  auto RegisterActionForCommand(const QString& command_id, QAction* action, const ContextList& visibility_contexts,
                                const ContextList& enabled_contexts) -> bool override;
  auto InsertMenuGroup(const QString& menu_id, const QString& group_id, int order) -> void override;
  auto FindMenu(const QString& menu_id) -> IActionContainer* override;
  auto FindCommand(const QString& command_id) -> ICommand* override;
  auto RegisterToolbar(const QString& toolbar_id, int order) -> IActionContainer* override;
  auto AddToolbarCommand(const QString& toolbar_id, const QString& command_id, const QString& group_id) -> void override;

 private:
  //! @cond

  QMap<QString, MenuDescriptor> registered_menus_;
  QMap<QString, MenuItemDescriptor> registered_items_;

  //! @endcond
};

}  // namespace sss::dscore
