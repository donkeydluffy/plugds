#include "CommandManager.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

#include "ActionContainer.h"
#include "Command.h"
#include "MainWindow.h"
#include "dscore/CoreConstants.h"
#include "dscore/IContextManager.h"
#include "dscore/ICore.h"

sss::dscore::CommandManager::CommandManager() {
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager != nullptr) {
    connect(context_manager, &sss::dscore::IContextManager::ContextChanged, this, &CommandManager::onContextChanged);
  }
}

sss::dscore::CommandManager::~CommandManager() {
  qDeleteAll(action_container_map_);
  qDeleteAll(command_map_);
}

auto sss::dscore::CommandManager::RegisterAction(QAction* action, QString id,
                                                 const sss::dscore::ContextList& visibility_contexts,
                                                 const sss::dscore::ContextList& enabled_contexts)
    -> sss::dscore::ICommand* {
  if (command_map_.contains(id)) {
    auto* command = command_map_[id];

    command->RegisterAction(action, visibility_contexts, enabled_contexts);
    command->SetContext(sss::dscore::IContextManager::GetInstance()->getActiveContexts());

    return command;
  }

  auto* command = new Command(id);

  command->RegisterAction(action, visibility_contexts, enabled_contexts);

  command->Action()->setText(action->text());

  command->SetContext(sss::dscore::IContextManager::GetInstance()->getActiveContexts());

  command_map_[id] = command;

  return command;
}

auto sss::dscore::CommandManager::RegisterAction(QAction* action, sss::dscore::ICommand* command,
                                                 const sss::dscore::ContextList& visibility_contexts,
                                                 const sss::dscore::ContextList& enabled_contexts) -> bool {
  auto* command_class = qobject_cast<sss::dscore::Command*>(command);

  if (command_class != nullptr) {
    command_class->RegisterAction(action, visibility_contexts, enabled_contexts);
    command_class->SetContext(sss::dscore::IContextManager::GetInstance()->getActiveContexts());
  }

  return false;
}

auto sss::dscore::CommandManager::SetContext(int context_id) -> void { onContextChanged(context_id, -1); }

void sss::dscore::CommandManager::onContextChanged(int new_context, int previous_context) {
  Q_UNUSED(new_context);
  Q_UNUSED(previous_context);

  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    return;
  }

  const sss::dscore::ContextList active_contexts = context_manager->getActiveContexts();

  auto command_iterator = QMapIterator<QString, Command*>(command_map_);

  while (command_iterator.hasNext()) {
    command_iterator.next();
    command_iterator.value()->SetContext(active_contexts);
  }
}

auto sss::dscore::CommandManager::CreatePopupMenu(const QString& identifier) -> sss::dscore::IActionContainer* {
  sss::dscore::ActionContainer* new_container;

  if (!identifier.isNull()) {
    if (action_container_map_.contains(identifier)) {
      return action_container_map_[identifier];
    }
  }

  new_container = new sss::dscore::ActionContainer(new QMenu);

  if (!identifier.isNull()) {
    action_container_map_[identifier] = new_container;

    connect(new_container, &QObject::destroyed, [this, identifier]() { action_container_map_.remove(identifier); });
  }

  return new_container;
}

auto sss::dscore::CommandManager::CreateMenu(const QString& identifier, IActionContainer* parent_container)
    -> sss::dscore::IActionContainer* {
  sss::dscore::ActionContainer* new_container;

  if (action_container_map_.contains(identifier)) {
    return action_container_map_[identifier];
  }

  if (parent_container == nullptr) {
    auto* main_window = sss::dscore::MainWindowInstance();
    main_window->menuBar()->show();
    new_container = new sss::dscore::ActionContainer(main_window->menuBar());
  } else {
    auto* parent = qobject_cast<sss::dscore::ActionContainer*>(parent_container);
    QMenuBar* parent_menu_bar = nullptr;

    if (parent->MenuBar() != nullptr) {
      parent_menu_bar = parent->MenuBar();
    }

    auto* menu = new QMenu(sss::dscore::constants::MenuText(identifier), parent_menu_bar);
    new_container = new sss::dscore::ActionContainer(menu);

    if (parent_menu_bar != nullptr) {
      parent_menu_bar->addAction(menu->menuAction());
    }
  }

  action_container_map_[identifier] = new_container;

  return new_container;
}

auto sss::dscore::CommandManager::CreateToolBar(const QString& identifier) -> sss::dscore::IActionContainer* {
  if (action_container_map_.contains(identifier)) {
    return action_container_map_[identifier];
  }

  auto* main_window = sss::dscore::MainWindowInstance();
  auto* tool_bar = new QToolBar(identifier);
  main_window->addToolBar(tool_bar);

  auto* new_container = new sss::dscore::ActionContainer(tool_bar);
  action_container_map_[identifier] = new_container;

  return new_container;
}

auto sss::dscore::CommandManager::FindMenu(const QString& identifier) -> sss::dscore::IActionContainer* {
  if (action_container_map_.contains(identifier)) {
    return action_container_map_[identifier];
  }

  return nullptr;
}

auto sss::dscore::CommandManager::FindCommand(const QString& identifier) -> sss::dscore::ICommand* {
  if (command_map_.contains(identifier)) {
    return command_map_[identifier];
  }

  return nullptr;
}
