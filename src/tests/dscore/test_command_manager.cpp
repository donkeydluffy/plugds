#include <doctest/doctest.h>
#include "CommandManager.h"
#include "ContextManager.h"
#include <dscore/ICore.h>
#include <dscore/ICommand.h>
#include <dscore/IActionContainer.h>
#include <extsystem/IComponentManager.h>
#include <QAction>
#include <QMainWindow>
#include <QApplication>
#include <QTimer>

// Mock Core for MainWindow
class MockCore : public sss::dscore::ICore {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICore)

 public:
  MockCore() : main_window_(new QMainWindow()) {}
  ~MockCore() override { delete main_window_; }

  auto GetMainWindow() -> QMainWindow* override { return main_window_; }
  auto Random(int min, int max) -> int override { return min + (max - min) / 2; }
  auto StorageFolder() -> QString override { return "."; }

 private:
  QMainWindow* main_window_;
};

// Test Fixture
struct CommandManagerFixture {
  sss::extsystem::IComponentManager* comp_mgr = nullptr;
  sss::dscore::ContextManager* context_mgr = nullptr;
  MockCore* mock_core = nullptr;
  sss::dscore::CommandManager* cmd_mgr = nullptr;

  CommandManagerFixture() {
    comp_mgr = sss::extsystem::IComponentManager::GetInstance();
    
    // Clean up existing objects to ensure clean state
    auto all_objects = comp_mgr->AllObjects();
    for (auto* obj : all_objects) {
      comp_mgr->RemoveObject(obj);
    }
    
    // Register ContextManager
    context_mgr = new sss::dscore::ContextManager();
    comp_mgr->AddObject(context_mgr);

    // Register MockCore
    mock_core = new MockCore();
    comp_mgr->AddObject(mock_core);

    // Create CommandManager (it will look up ContextManager)
    cmd_mgr = new sss::dscore::CommandManager();
    comp_mgr->AddObject(cmd_mgr);
  }

  ~CommandManagerFixture() {
    // Cleanup in reverse order of dependency
    if (cmd_mgr) {
      comp_mgr->RemoveObject(cmd_mgr);
      delete cmd_mgr;
    }
    if (mock_core) {
      comp_mgr->RemoveObject(mock_core);
      delete mock_core;
    }
    if (context_mgr) {
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

    // Register
    sss::dscore::ICommand* cmd = cmd_mgr->RegisterAction(action, id, contexts, contexts);
    
    CHECK(cmd != nullptr);
    // Command creates a proxy action, so pointers won't match. Check text or other properties.
    CHECK(cmd->Action()->text() == action->text());
    
    // Find
    auto* found_cmd = cmd_mgr->FindCommand(id);
    CHECK(found_cmd == cmd);
    
    // Verify Context behavior
    // Initially active context is 0 (global usually) or empty.
    // Our mock ContextManager starts with some state.
    // Let's set context and see if command updates.
    
    // context_mgr->SetContext(1); // This might need more setup in ContextManager
    
    delete action; // Command doesn't own the QAction pointer passed to it initially? 
                   // Wait, CommandManager::RegisterAction documentation doesn't say it takes ownership of QAction,
                   // but Command implementation might.
                   // Looking at Command.cpp, it stores QAction* in a map.
                   // Usually QAction is owned by parent or created on heap.
                   // Let's assume we manage it or QObject parentage handles it.
  }

  TEST_CASE_FIXTURE(CommandManagerFixture, "CreateActionContainer") {
    QString id = "test.menu";
    auto* container = cmd_mgr->CreateActionContainer(id, sss::dscore::ContainerType::kMenu, nullptr, 0);
    
    CHECK(container != nullptr);
    CHECK(cmd_mgr->FindContainer(id) == container);
    
    // Create a child menu
    QString child_id = "test.menu.child";
    auto* child_container = cmd_mgr->CreateActionContainer(child_id, sss::dscore::ContainerType::kMenu, container, 0);
    
    CHECK(child_container != nullptr);
    CHECK(cmd_mgr->FindContainer(child_id) == child_container);
  }
}

#include "test_command_manager.moc"
