#include <doctest/doctest.h>
#include <dscore/IActionContainer.h>
#include <dscore/ICommand.h>
#include <dscore/ICore.h>
#include <extsystem/IComponentManager.h>

#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QTimer>

#include "CommandManager.h"
#include "ContextManager.h"

// MainWindow的模拟Core
class MockCore : public sss::dscore::ICore {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICore)

 public:
  MockCore() : main_window_(new QMainWindow()) {}
  ~MockCore() override { delete main_window_; }

  auto GetMainWindow() -> QMainWindow* override { return main_window_; }
  auto Random(int min, int max) -> int override { return min + ((max - min) / 2); }
  auto StorageFolder() -> QString override { return "."; }

 private:
  QMainWindow* main_window_;
};

// 测试夹具
struct CommandManagerFixture {
  sss::extsystem::IComponentManager* comp_mgr = nullptr;  // NOLINT
  sss::dscore::ContextManager* context_mgr = nullptr;     // NOLINT
  MockCore* mock_core = nullptr;                          // NOLINT
  sss::dscore::CommandManager* cmd_mgr = nullptr;         // NOLINT

  CommandManagerFixture() {
    comp_mgr = sss::extsystem::IComponentManager::GetInstance();

    // 清理现有对象以确保清洁状态
    auto all_objects = comp_mgr->AllObjects();
    for (auto* obj : all_objects) {
      comp_mgr->RemoveObject(obj);
    }

    // 注册上下文管理器
    context_mgr = new sss::dscore::ContextManager();
    comp_mgr->AddObject(context_mgr);

    // 注册模拟Core
    mock_core = new MockCore();
    comp_mgr->AddObject(mock_core);

    // 创建命令管理器（它会查找上下文管理器）
    cmd_mgr = new sss::dscore::CommandManager();
    comp_mgr->AddObject(cmd_mgr);
  }

  ~CommandManagerFixture() {
    // 按依赖关系的相反顺序清理
    if (cmd_mgr != nullptr) {
      comp_mgr->RemoveObject(cmd_mgr);
      delete cmd_mgr;
    }
    if (mock_core != nullptr) {
      comp_mgr->RemoveObject(mock_core);
      delete mock_core;
    }
    if (context_mgr != nullptr) {
      comp_mgr->RemoveObject(context_mgr);
      delete context_mgr;
    }
  }
};

TEST_SUITE("CommandManager") {
  TEST_CASE_FIXTURE(CommandManagerFixture, "RegisterAction and Retrieval") {
    auto* action = new QAction("Test Action", nullptr);
    QString id = "test.action";
    sss::dscore::ContextList contexts = {1};

    // 注册
    sss::dscore::ICommand* cmd = cmd_mgr->RegisterAction(action, id, contexts, contexts);

    CHECK(cmd != nullptr);
    // 命令创建一个代理动作，所以指针不会匹配。检查文本或其他属性。
    CHECK(cmd->Action()->text() == action->text());

    // 查找
    auto* found_cmd = cmd_mgr->FindCommand(id);
    CHECK(found_cmd == cmd);

    // 验证上下文行为
    // 初始活动上下文为0（通常是全局的）或为空。
    // 模拟上下文管理器以某种状态开始。
    // 设置上下文并查看命令是否更新。

    // context_mgr->SetContext(1); // 这可能需要在ContextManager中进行更多设置

    delete action;  // 命令不拥有最初传递给它的QAction指针？
                    // 通常QAction由父对象拥有或在堆上创建。
                    // 我们假设我们管理它或QObject父子关系处理它。
  }

  TEST_CASE_FIXTURE(CommandManagerFixture, "CreateActionContainer") {
    QString id = "test.menu";
    auto* container = cmd_mgr->CreateActionContainer(id, sss::dscore::ContainerType::kMenu, nullptr, 0);

    CHECK(container != nullptr);
    CHECK(cmd_mgr->FindContainer(id) == container);

    // 创建一个子菜单
    QString child_id = "test.menu.child";
    auto* child_container = cmd_mgr->CreateActionContainer(child_id, sss::dscore::ContainerType::kMenu, container, 0);

    CHECK(child_container != nullptr);
    CHECK(cmd_mgr->FindContainer(child_id) == child_container);
  }
}

#include "test_command_manager.moc"
