#include "ActionContainer.h"

#include <cassert>

#include "dscore/CoreConstants.h"
#include "dscore/ICommand.h"
#include "dscore/ICommandManager.h"

sss::dscore::ActionContainer::ActionContainer() {
  group_list_.append(GroupItem(sss::dscore::constants::menugroups::kTop));
  group_list_.append(GroupItem(sss::dscore::constants::menugroups::kMiddle));
  group_list_.append(GroupItem(sss::dscore::constants::menugroups::kBottom));
}

sss::dscore::ActionContainer::~ActionContainer() = default;

sss::dscore::ActionContainer::ActionContainer(QMenuBar* menu_bar) : ActionContainer() { menu_bar_ = menu_bar; }

sss::dscore::ActionContainer::ActionContainer(QMenu* menu) : ActionContainer() { menu_ = menu; }

sss::dscore::ActionContainer::ActionContainer(QToolBar* tool_bar) : ActionContainer() { tool_bar_ = tool_bar; }

auto sss::dscore::ActionContainer::Type() -> sss::dscore::ActionContainerTypes {
  if (tool_bar_ != nullptr) {
    return sss::dscore::ActionContainerTypes::kIsToolBar;
  }
  if (menu_bar_ != nullptr) {
    return sss::dscore::ActionContainerTypes::kIsMenuBar;
  }
  if ((menu_ != nullptr) && (qobject_cast<QMenuBar*>(menu_->parent()) == nullptr)) {
    return sss::dscore::ActionContainerTypes::kIsSubMenu;
  }
  return sss::dscore::ActionContainerTypes::kIsMenu;
}

auto sss::dscore::ActionContainer::GetMenu() -> QMenu* { return menu_; }

auto sss::dscore::ActionContainer::MenuBar() -> QMenuBar* { return menu_bar_; }

auto sss::dscore::ActionContainer::toolBar() -> QToolBar* { return tool_bar_; }

auto sss::dscore::ActionContainer::getInsertAction(QList<ActionContainer::GroupItem>::const_iterator group_iterator)
    -> QAction* {
  if (group_iterator->items_.count() != 0) {
    auto* cast_to_command = qobject_cast<sss::dscore::ICommand*>(group_iterator->items_.first());

    if (cast_to_command != nullptr) {
      return cast_to_command->Action();
    }
  }

  while (group_iterator != group_list_.constEnd()) {
    if (group_iterator->items_.count() != 0) {
      break;
    }

    group_iterator++;
  }

  if (group_iterator == group_list_.constEnd()) {
    return nullptr;
  }

  if (group_iterator->items_.count() != 0) {
    auto* cast_to_command = qobject_cast<sss::dscore::ICommand*>(group_iterator->items_.first());

    if (cast_to_command != nullptr) {
      return cast_to_command->Action();
    }
  }

  return nullptr;
}

auto sss::dscore::ActionContainer::getAppendAction(QList<ActionContainer::GroupItem>::const_iterator group_iterator)
    -> QAction* {
  group_iterator++;

  while (group_iterator != group_list_.constEnd()) {
    if (group_iterator->items_.count() != 0) {
      break;
    }

    group_iterator++;
  }

  if (group_iterator == group_list_.constEnd()) {
    return nullptr;
  }

  if (group_iterator->items_.count() != 0) {
    auto* cast_to_command = qobject_cast<sss::dscore::ICommand*>(group_iterator->items_.first());

    if (cast_to_command != nullptr) {
      return cast_to_command->Action();
    }
  }

  return nullptr;
}

auto sss::dscore::ActionContainer::InsertCommand(sss::dscore::ICommand* command, QString group) -> void {
  if (command == nullptr) {
    return;
  }

  // For toolbars, insert is the same as append.
  if (tool_bar_ != nullptr) {
    AppendCommand(command, group);
    return;
  }

  if (menu_ == nullptr) {
    return;
  }

  auto group_iterator = findGroup(group);

  if (group_iterator == group_list_.constEnd()) {
    return;
  }

  auto* next_action = getInsertAction(group_iterator);

  menu_->insertAction(next_action, command->Action());

  group_list_[group_iterator - group_list_.constBegin()].items_.append(command);
}

auto sss::dscore::ActionContainer::AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void {
  if (command == nullptr) {
    return;
  }

  auto group_iterator = findGroup(group_identifier);
  if (group_iterator == group_list_.constEnd()) {
    return;
  }

  if (tool_bar_ != nullptr) {
    if (group_list_[group_iterator - group_list_.constBegin()].items_.count() == 0 &&
        group_iterator != group_list_.constBegin()) {
      tool_bar_->addSeparator();
    }
    tool_bar_->addAction(command->Action());
    group_list_[group_iterator - group_list_.constBegin()].items_.append(command);
    return;
  }

  if (menu_ == nullptr) {
    return;
  }

  auto* previous_action = getAppendAction(group_iterator);

  QList<QAction*> action_list;

  if (group_list_[group_iterator - group_list_.constBegin()].items_.count() == 0 &&
      group_iterator != group_list_.constBegin()) {
    menu_->insertSeparator(previous_action);

    auto* separator_action = new QAction();

    separator_action->setSeparator(true);

    action_list.append(separator_action);
  }

  action_list.append(command->Action());

  menu_->insertActions(previous_action, action_list);

  group_list_[group_iterator - group_list_.constBegin()].items_.append(command);
}

auto sss::dscore::ActionContainer::findGroup(const QString& group_identifier)
    -> QList<ActionContainer::GroupItem>::const_iterator {
  QList<ActionContainer::GroupItem>::const_iterator group_iterator = group_list_.constBegin();

  while (group_iterator != group_list_.constEnd()) {
    if (group_iterator->id_ == group_identifier) {
      break;
    }

    group_iterator++;
  }

  return group_iterator;
}

auto sss::dscore::ActionContainer::AddGroupAfter(QString after_identifier, QString group_identifier) -> bool {
  auto group_iterator = findGroup(after_identifier);

  if (group_iterator == group_list_.constEnd()) {
    group_list_.append(GroupItem(group_identifier));

    return true;
  }

  group_list_.insert((group_iterator - group_list_.constBegin()) + 1, GroupItem(group_identifier));

  return true;
}

auto sss::dscore::ActionContainer::AddGroupBefore(QString before_identifier, QString group_identifier) -> bool {
  auto group_iterator = findGroup(before_identifier);

  if (group_iterator == group_list_.constEnd()) {
    return false;
  }

  if (group_iterator == group_list_.constBegin()) {
    group_list_.insert(0, GroupItem(group_identifier));

    return true;
  }

  group_list_.insert(group_iterator - group_list_.constBegin(), GroupItem(group_identifier));

  return true;
}

auto sss::dscore::ActionContainer::AppendGroup(QString group_identifier) -> void {
  group_list_.append(GroupItem(group_identifier));
}

auto sss::dscore::ActionContainer::InsertGroup(QString group_identifier) -> void {
  group_list_.insert(0, GroupItem(group_identifier));
}

auto sss::dscore::ActionContainer::AppendCommand(QString command_identifier, QString group_identifier) -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  assert(command_manager != nullptr);

  auto* command = command_manager->FindCommand(command_identifier);

  if (command != nullptr) {
    AppendCommand(command, group_identifier);
  }
}

auto sss::dscore::ActionContainer::InsertCommand(QString command_identifier, QString group_identifier) -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  assert(command_manager != nullptr);

  auto* command = command_manager->FindCommand(command_identifier);

  if (command != nullptr) {
    InsertCommand(command, group_identifier);
  }
}
