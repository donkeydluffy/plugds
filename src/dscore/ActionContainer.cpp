#include "ActionContainer.h"

#include <cassert>

#include "dscore/ICommand.h"
#include "dscore/ICommandManager.h"

sss::dscore::ActionContainer::ActionContainer() = default;

sss::dscore::ActionContainer::~ActionContainer() = default;

sss::dscore::ActionContainer::ActionContainer(QMenuBar* menu_bar) : ActionContainer() { menu_bar_ = menu_bar; }

sss::dscore::ActionContainer::ActionContainer(QMenu* menu) : ActionContainer() { menu_ = menu; }

sss::dscore::ActionContainer::ActionContainer(QToolBar* tool_bar) : ActionContainer() { tool_bar_ = tool_bar; }

auto sss::dscore::ActionContainer::GetType() -> sss::dscore::ContainerType {
  if (tool_bar_ != nullptr) {
    return sss::dscore::ContainerType::kToolBar;
  }
  if (menu_bar_ != nullptr) {
    return sss::dscore::ContainerType::kMenuBar;
  }
  if ((menu_ != nullptr) && (qobject_cast<QMenuBar*>(menu_->parent()) == nullptr)) {
    // 如果将来需要，使用功能区栏逻辑，目前为子菜单
    return sss::dscore::ContainerType::kMenu;
  }
  return sss::dscore::ContainerType::kMenu;
}

auto sss::dscore::ActionContainer::GetWidget() -> QWidget* {
  if (menu_bar_ != nullptr) return menu_bar_;
  if (tool_bar_ != nullptr) return tool_bar_;
  return menu_;
}

auto sss::dscore::ActionContainer::getInsertAction(QList<ActionContainer::GroupItem>::iterator group_iterator)
    -> QAction* {
  if (group_iterator->items_.count() != 0) {
    auto* cast_to_command = qobject_cast<sss::dscore::ICommand*>(group_iterator->items_.first());

    if (cast_to_command != nullptr) {
      return cast_to_command->Action();
    }
  }

  while (group_iterator != group_list_.end()) {
    if (group_iterator->items_.count() != 0) {
      break;
    }

    group_iterator++;
  }

  if (group_iterator == group_list_.end()) {
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

auto sss::dscore::ActionContainer::getAppendAction(QList<ActionContainer::GroupItem>::iterator group_iterator)
    -> QAction* {
  group_iterator++;

  while (group_iterator != group_list_.end()) {
    if (group_iterator->items_.count() != 0) {
      break;
    }

    group_iterator++;
  }

  if (group_iterator == group_list_.end()) {
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

auto sss::dscore::ActionContainer::AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void {
  if (command == nullptr) {
    return;
  }

  auto group_iterator = findGroup(group_identifier);
  if (group_iterator == group_list_.end()) {
    return;
  }

  if (tool_bar_ != nullptr) {
    if (group_list_[group_iterator - group_list_.begin()].items_.count() == 0 &&
        group_iterator != group_list_.begin()) {
      tool_bar_->addSeparator();
    }
    tool_bar_->addAction(command->Action());
    group_list_[group_iterator - group_list_.begin()].items_.append(command);
    return;
  }

  if (menu_ == nullptr) {
    return;
  }

  auto* previous_action = getAppendAction(group_iterator);

  QList<QAction*> action_list;

  if (group_list_[group_iterator - group_list_.begin()].items_.count() == 0 && group_iterator != group_list_.begin()) {
    menu_->insertSeparator(previous_action);

    auto* separator_action = new QAction();

    separator_action->setSeparator(true);

    action_list.append(separator_action);
  }

  action_list.append(command->Action());

  menu_->insertActions(previous_action, action_list);

  group_list_[group_iterator - group_list_.begin()].items_.append(command);
}

auto sss::dscore::ActionContainer::findGroup(const QString& group_identifier)
    -> QList<ActionContainer::GroupItem>::iterator {
  QList<ActionContainer::GroupItem>::iterator group_iterator = group_list_.begin();

  while (group_iterator != group_list_.end()) {
    if (group_iterator->id_ == group_identifier) {
      break;
    }

    group_iterator++;
  }

  return group_iterator;
}

auto sss::dscore::ActionContainer::InsertGroup(QString group_identifier, int order) -> void {
  // 基于优先级的插入
  auto it = group_list_.begin();
  while (it != group_list_.end()) {
    if (it->order_ > order) {
      break;
    }
    it++;
  }
  group_list_.insert(it, GroupItem(group_identifier, order));
}

auto sss::dscore::ActionContainer::AppendCommand(QString command_identifier, QString group_identifier) -> void {
  auto* command_manager = sss::dscore::ICommandManager::GetInstance();

  assert(command_manager != nullptr);

  auto* command = command_manager->FindCommand(command_identifier);

  if (command != nullptr) {
    AppendCommand(command, group_identifier);
  }
}
