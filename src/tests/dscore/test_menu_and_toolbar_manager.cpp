#include <doctest/doctest.h>
#include <QSignalSpy>
#include <dscore/MenuAndToolbarManager.h>
#include <dscore/ICommandManager.h>
#include <dscore/IMenuProvider.h>
#include <dscore/IToolbarProvider.h>
#include <dscore/ICommandProvider.h>
#include <extsystem/IComponentManager.h>
#include <dscore/CoreConstants.h>

// Mock Command Manager
class MockCommandManager : public sss::dscore::ICommandManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandManager)
 public:
  // Mock methods
  auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* override {
      (void)action; (void)id; (void)visibility_contexts; (void)enabled_contexts;
      return nullptr;
  }
  auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                              const sss::dscore::ContextList& visibility_contexts,
                              const sss::dscore::ContextList& enabled_contexts) -> bool override {
       (void)action; (void)command; (void)visibility_contexts; (void)enabled_contexts;
       return true;
  }
  auto SetContext(int context_id) -> void override { (void)context_id; }
  
  auto CreateActionContainer(const QString& identifier, sss::dscore::ContainerType type,
                                     sss::dscore::IActionContainer* parent_container, int order)
      -> sss::dscore::IActionContainer* override {
      createContainerCalls++;
      createdContainers.append(identifier);
      (void)type; (void)parent_container; (void)order;
      return nullptr; // Return null or mock container if needed, but for this test null might be okay if not dereferenced
  }
  
  auto FindContainer(const QString& identifier) -> sss::dscore::IActionContainer* override {
      // Return null to simulate container not existing, triggering creation
      (void)identifier;
      return nullptr;
  }
  
  auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* override {
      (void)identifier;
      return nullptr;
  }
  
  auto RetranslateUi() -> void override {
      retranslateCalls++;
  }

  int createContainerCalls = 0;
  int retranslateCalls = 0;
  QStringList createdContainers;
};

// Mock Menu Provider
class MockMenuProvider : public QObject, public sss::dscore::IMenuProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IMenuProvider)
 public:
  void ContributeToMenu(sss::dscore::ICommandManager* command_manager) override {
      (void)command_manager;
      contributeCalls++;
  }
  int contributeCalls = 0;
};

// Mock Toolbar Provider
class MockToolbarProvider : public QObject, public sss::dscore::IToolbarProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IToolbarProvider)
 public:
  void ContributeToToolbar(sss::dscore::ICommandManager* command_manager) override {
      (void)command_manager;
      contributeCalls++;
  }
  int contributeCalls = 0;
};

// Mock Command Provider
class MockCommandProvider : public QObject, public sss::dscore::ICommandProvider {
    Q_OBJECT
    Q_INTERFACES(sss::dscore::ICommandProvider)
public:
    void RegisterCommands(sss::dscore::ICommandManager* command_manager) override {
        (void)command_manager;
        registerCalls++;
    }
    int registerCalls = 0;
};

struct MenuToolbarFixture {
    sss::extsystem::IComponentManager* comp_mgr;
    MockCommandManager* cmd_mgr;
    
    MenuToolbarFixture() {
        comp_mgr = sss::extsystem::IComponentManager::GetInstance();
        auto all = comp_mgr->AllObjects();
        for(auto* o : all) comp_mgr->RemoveObject(o);
        
        cmd_mgr = new MockCommandManager();
        comp_mgr->AddObject(cmd_mgr);
    }
    
    ~MenuToolbarFixture() {
        comp_mgr->RemoveObject(cmd_mgr);
        delete cmd_mgr;
    }
};

TEST_SUITE("MenuAndToolbarManager") {
    TEST_CASE_FIXTURE(MenuToolbarFixture, "Build invokes providers") {
        sss::dscore::MenuAndToolbarManager manager;
        
        MockMenuProvider menuProvider;
        MockToolbarProvider toolbarProvider;
        MockCommandProvider commandProvider;
        
        comp_mgr->AddObject(&menuProvider);
        comp_mgr->AddObject(&toolbarProvider);
        comp_mgr->AddObject(&commandProvider);
        
        manager.Build();
        
        CHECK(cmd_mgr->createContainerCalls >= 1); // Should create App Menu
        CHECK(cmd_mgr->createdContainers.contains(sss::dscore::constants::menubars::kApplication));
        
        CHECK(menuProvider.contributeCalls == 1);
        CHECK(toolbarProvider.contributeCalls == 1);
        CHECK(commandProvider.registerCalls == 1);
        CHECK(cmd_mgr->retranslateCalls == 1);
        
        comp_mgr->RemoveObject(&menuProvider);
        comp_mgr->RemoveObject(&toolbarProvider);
        comp_mgr->RemoveObject(&commandProvider);
    }
}

#include "test_menu_and_toolbar_manager.moc"
