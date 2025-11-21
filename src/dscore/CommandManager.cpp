#include "CommandManager.h"

#include <QMenu>
#include <QMenuBar>

#include "Command.h"
#include "dscore/CoreConstants.h"
#include "dscore/ICore.h"

sss::dscore::CommandManager::CommandManager() = default;

sss::dscore::CommandManager::~CommandManager() {
  qDeleteAll(menu_map_);
  qDeleteAll(command_map_);
}

auto sss::dscore::CommandManager::RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& contexts)
    -> sss::dscore::ICommand* {
  if (command_map_.contains(id)) {
    auto* command = command_map_[id];

    command->RegisterAction(action, contexts);
    command->SetContext(sss::dscore::IContextManager::GetInstance()->Context());

    command->SetActive(action->isEnabled());

    return command;
  }

  auto* command = new Command(id);

  command->RegisterAction(action, contexts);

  command->Action()->setText(action->text());

  command->SetContext(sss::dscore::IContextManager::GetInstance()->Context());

  command_map_[id] = command;

  return command;
}

auto sss::dscore::CommandManager::RegisterAction(QAction* action, sss::dscore::ICommand* command,
                                                 const sss::dscore::ContextList& contexts) -> bool {
  auto* command_class = qobject_cast<sss::dscore::Command*>(command);

  if (command_class != nullptr) {
    command_class->RegisterAction(action, contexts);
    command_class->SetContext(sss::dscore::IContextManager::GetInstance()->Context());

    command_class->SetActive(action->isEnabled());
  }

  return false;
}

auto sss::dscore::CommandManager::SetContext(int context_id) -> void {
  auto command_iterator = QMapIterator<QString, Command*>(command_map_);

  while (command_iterator.hasNext()) {
    command_iterator.next();

    command_iterator.value()->SetContext(context_id);
  }
}

auto sss::dscore::CommandManager::CreatePopupMenu(const QString& identifier) -> sss::dscore::IMenu* {
  sss::dscore::Menu* new_menu;

  if (!identifier.isNull()) {
    if (menu_map_.contains(identifier)) {
      return menu_map_[identifier];
    }
  }

  new_menu = new sss::dscore::Menu(new QMenu);

  if (!identifier.isNull()) {
    menu_map_[identifier] = new_menu;

    connect(new_menu, &QObject::destroyed, [this, identifier]() { menu_map_.remove(identifier); });
  }

  return new_menu;
}

auto sss::dscore::CommandManager::CreateMenu(const QString& identifier, IMenu* parent_menu) -> sss::dscore::IMenu* {
  sss::dscore::Menu* new_menu;

  if (menu_map_.contains(identifier)) {
    return menu_map_[identifier];
  }

  if (parent_menu == nullptr) {
    auto* main_window = sss::dscore::MainWindowInstance();

    main_window->menuBar()->show();

    new_menu = new sss::dscore::Menu(main_window->menuBar());
  } else {
    auto* parent = qobject_cast<sss::dscore::Menu*>(parent_menu);

    QMenuBar* parent_menu_bar = nullptr;

    if (parent->menu_bar_ != nullptr) {
      parent_menu_bar = parent->menu_bar_;
    }

    auto* menu = new QMenu(sss::dscore::constants::MenuText(identifier), parent_menu_bar);

    new_menu = new sss::dscore::Menu(menu);

    if (parent_menu_bar != nullptr) {
      parent_menu_bar->addAction(menu->menuAction());
    }
  }

  menu_map_[identifier] = new_menu;

  return new_menu;
}

auto sss::dscore::CommandManager::FindMenu(const QString& identifier) -> sss::dscore::IMenu* {
  if (menu_map_.contains(identifier)) {
    return menu_map_[identifier];
  }

  return nullptr;
}

auto sss::dscore::CommandManager::FindCommand(const QString& identifier) -> sss::dscore::ICommand* {
  if (command_map_.contains(identifier)) {
    return command_map_[identifier];
  }

  return nullptr;
}
