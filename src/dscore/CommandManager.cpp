#include "CommandManager.h"

#include <spdlog/spdlog.h>

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
    command->SetContext(sss::dscore::IContextManager::GetInstance()->GetActiveContexts());

    return command;
  }

  auto* command = new Command(id);

  command->RegisterAction(action, visibility_contexts, enabled_contexts);

  command->Action()->setText(action->text());

  command->SetContext(sss::dscore::IContextManager::GetInstance()->GetActiveContexts());

  command_map_[id] = command;

  return command;
}

auto sss::dscore::CommandManager::RegisterAction(QAction* action, sss::dscore::ICommand* command,
                                                 const sss::dscore::ContextList& visibility_contexts,
                                                 const sss::dscore::ContextList& enabled_contexts) -> bool {
  auto* command_class = qobject_cast<sss::dscore::Command*>(command);

  if (command_class != nullptr) {
    command_class->RegisterAction(action, visibility_contexts, enabled_contexts);
    command_class->SetContext(sss::dscore::IContextManager::GetInstance()->GetActiveContexts());
  }

  return false;
}

auto sss::dscore::CommandManager::SetContext(int context_id) -> void { onContextChanged(context_id, -1); }

void sss::dscore::CommandManager::onContextChanged(int new_context, int previous_context) {
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    return;
  }

  const sss::dscore::ContextList active_contexts = context_manager->GetActiveContexts();

  auto command_iterator = QMapIterator<QString, Command*>(command_map_);

  while (command_iterator.hasNext()) {
    command_iterator.next();
    command_iterator.value()->SetContext(active_contexts);
  }
}

auto sss::dscore::CommandManager::CreateActionContainer(const QString& identifier, sss::dscore::ContainerType type,
                                                        IActionContainer* parent_container, int order)
    -> sss::dscore::IActionContainer* {
  switch (type) {
    case sss::dscore::ContainerType::kMenuBar:
    case sss::dscore::ContainerType::kMenu:
      return createMenu(identifier, parent_container, order);
    case sss::dscore::ContainerType::kToolBar:
      return createToolBar(identifier, order);
    default:
      SPDLOG_WARN("Unsupported container type requested: {}", static_cast<int>(type));
      return nullptr;
  }
}

auto sss::dscore::CommandManager::createMenu(const QString& identifier, IActionContainer* parent_container, int order)
    -> sss::dscore::IActionContainer* {
  sss::dscore::ActionContainer* new_container;

  if (action_container_map_.contains(identifier)) {
    return action_container_map_[identifier];
  }

  if (parent_container == nullptr) {
    // 包装主菜单栏
    auto* main_window = sss::dscore::MainWindowInstance();
    main_window->menuBar()->show();
    new_container = new sss::dscore::ActionContainer(main_window->menuBar());
    new_container->SetOrder(order);
  } else {
    // 创建子菜单（或菜单栏中的顶级菜单项）
    auto* parent = qobject_cast<sss::dscore::ActionContainer*>(parent_container);
    QMenuBar* parent_menu_bar = nullptr;
    if (parent != nullptr) {
      parent_menu_bar = qobject_cast<QMenuBar*>(parent->GetWidget());
    }

    // 如果父级不是菜单栏，则可能是菜单（子菜单）
    QMenu* parent_menu = nullptr;
    if (parent_menu_bar == nullptr && parent != nullptr) {
      parent_menu = qobject_cast<QMenu*>(parent->GetWidget());
    }

    auto* menu = new QMenu(sss::dscore::constants::MenuText(identifier), (parent_menu_bar != nullptr)
                                                                             ? static_cast<QWidget*>(parent_menu_bar)
                                                                             : static_cast<QWidget*>(parent_menu));
    menu->menuAction()->setData(order);  // 在 QAction 中存储排序值

    new_container = new sss::dscore::ActionContainer(menu);
    new_container->SetOrder(order);

    if (parent_menu_bar != nullptr) {
      // 按排序插入菜单栏
      QAction* before = nullptr;
      for (auto* action : parent_menu_bar->actions()) {
        bool ok = false;
        int action_order = action->data().toInt(&ok);
        if (ok && action_order > order) {
          before = action;
          break;
        }
      }
      parent_menu_bar->insertAction(before, menu->menuAction());
    } else if (parent_menu != nullptr) {
      // 按排序插入父级菜单
      QAction* before = nullptr;
      for (auto* action : parent_menu->actions()) {
        bool ok = false;
        int action_order = action->data().toInt(&ok);
        // 只与设置了排序的动作进行比较（子菜单/分组）
        if (ok && action_order > order) {
          before = action;
          break;
        }
      }
      parent_menu->insertAction(before, menu->menuAction());
    }
  }

  action_container_map_[identifier] = new_container;

  return new_container;
}

auto sss::dscore::CommandManager::createToolBar(const QString& identifier, int order)
    -> sss::dscore::IActionContainer* {
  if (action_container_map_.contains(identifier)) {
    return action_container_map_[identifier];
  }

  auto* main_window = sss::dscore::MainWindowInstance();

  // 创建工具栏
  auto* tool_bar = new QToolBar(identifier);
  tool_bar->setProperty("order", order);

  // 按排序插入
  QToolBar* before = nullptr;
  for (auto* existing_toolbar : main_window->findChildren<QToolBar*>()) {
    // 检查是否为主窗口区域的工具栏
    if (main_window->toolBarArea(existing_toolbar) == Qt::NoToolBarArea) continue;

    bool ok = false;
    int existing_order = existing_toolbar->property("order").toInt(&ok);
    if (ok && existing_order > order) {
      before = existing_toolbar;
      break;
    }
  }

  if (before != nullptr) {
    main_window->insertToolBar(before, tool_bar);
  } else {
    main_window->addToolBar(tool_bar);
  }

  auto* new_container = new sss::dscore::ActionContainer(tool_bar);
  new_container->SetOrder(order);
  action_container_map_[identifier] = new_container;

  return new_container;
}

auto sss::dscore::CommandManager::FindContainer(const QString& identifier) -> sss::dscore::IActionContainer* {
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

auto sss::dscore::CommandManager::RetranslateUi() -> void {
  // 更新命令
  for (auto it = command_map_.begin(); it != command_map_.end(); ++it) {
    const QString& id = it.key();
    ICommand* command = it.value();
    if ((command != nullptr) && (command->Action() != nullptr)) {
      // 只更新标准命令或我们已知 ID 对应翻译字符串的命令。
      // CommandText() 处理在核心字符串集合中的查找或回退处理。
      // 我们假设非核心命令可能只返回 ID 或未映射时的原始文本，
      // 但由于 CommandText 回退到使用 QT_TR_NOOP 的 kMap 查找，如果上下文匹配，它*可能*会重新翻译。
      // 然而，主要目标是修复核心命令（文件、打开等）。
      QString text = sss::dscore::constants::CommandText(id);
      // 如果 CommandText 返回 ID 本身（回退），我们可能不应该覆盖
      // 除非我们确定它不是用户字符串。
      // 但 CommandText 实现返回：
      // 1. 核心字符串集合值（已翻译）
      // 2. QObject::tr(kMap 值)（如果上下文匹配则翻译）
      // 3. 字符串（ID）

      if (text != id) {
        command->Action()->setText(text);
      }
    }
  }

  // 更新菜单/容器
  for (auto it = action_container_map_.begin(); it != action_container_map_.end(); ++it) {
    const QString& id = it.key();
    IActionContainer* container = it.value();
    QString text = sss::dscore::constants::MenuText(id);

    if (text != id && (container->GetWidget() != nullptr)) {
      if (auto* menu = qobject_cast<QMenu*>(container->GetWidget())) {
        menu->setTitle(text);
      } else if (auto* toolbar = qobject_cast<QToolBar*>(container->GetWidget())) {
        toolbar->setWindowTitle(text);
      }
    }
  }
}
