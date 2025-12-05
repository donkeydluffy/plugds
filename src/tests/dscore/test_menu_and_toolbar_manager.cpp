#include <doctest/doctest.h>
#include <dscore/CoreConstants.h>
#include <dscore/ICommandManager.h>
#include <dscore/ICommandProvider.h>
#include <dscore/IMenuProvider.h>
#include <dscore/IToolbarProvider.h>
#include <dscore/MenuAndToolbarManager.h>
#include <extsystem/IComponentManager.h>

#include <QSignalSpy>

// Mock Command Manager
class MockCommandManager : public sss::dscore::ICommandManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandManager)
 public:
  // Mock methods
  auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* override {
    (void)action;
    (void)id;
    (void)visibility_contexts;
    (void)enabled_contexts;
    return nullptr;
  }
  auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                      const sss::dscore::ContextList& visibility_contexts,
                      const sss::dscore::ContextList& enabled_contexts) -> bool override {
    (void)action;
    (void)command;
    (void)visibility_contexts;
    (void)enabled_contexts;
    return true;
  }
  auto SetContext(int context_id) -> void override { (void)context_id; }

  auto CreateActionContainer(const QString& identifier, sss::dscore::ContainerType type,
                             sss::dscore::IActionContainer* parent_container, int order)
      -> sss::dscore::IActionContainer* override {
    create_container_calls++;
    created_containers.append(identifier);
    (void)type;
    (void)parent_container;
    (void)order;
    return nullptr;  // Return null or mock container if needed, but for this test null might be okay if not
                     // dereferenced
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

  auto RetranslateUi() -> void override { retranslate_calls++; }

  int create_container_calls = 0;  // NOLINT
  int retranslate_calls = 0;       // NOLINT
  QStringList created_containers;  // NOLINT
};

// Mock Menu Provider
class MockMenuProvider : public QObject, public sss::dscore::IMenuProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IMenuProvider)
 public:
  void ContributeToMenu(sss::dscore::ICommandManager* command_manager) override {
    (void)command_manager;
    contribute_calls++;
  }
  int contribute_calls = 0;  // NOLINT
};

// Mock Toolbar Provider
class MockToolbarProvider : public QObject, public sss::dscore::IToolbarProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IToolbarProvider)
 public:
  void ContributeToToolbar(sss::dscore::ICommandManager* command_manager) override {
    (void)command_manager;
    contribute_calls++;
  }
  int contribute_calls = 0;  // NOLINT
};

// Mock Command Provider
class MockCommandProvider : public QObject, public sss::dscore::ICommandProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandProvider)
 public:
  void RegisterCommands(sss::dscore::ICommandManager* command_manager) override {
    (void)command_manager;
    register_calls++;
  }
  int register_calls = 0;  // NOLINT
};

struct MenuToolbarFixture {
  sss::extsystem::IComponentManager* comp_mgr;  // NOLINT
  MockCommandManager* cmd_mgr;                  // NOLINT

  MenuToolbarFixture() {
    comp_mgr = sss::extsystem::IComponentManager::GetInstance();
    auto all = comp_mgr->AllObjects();
    for (auto* o : all) comp_mgr->RemoveObject(o);

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

    MockMenuProvider menu_provider;
    MockToolbarProvider toolbar_provider;
    MockCommandProvider command_provider;

    comp_mgr->AddObject(&menu_provider);
    comp_mgr->AddObject(&toolbar_provider);
    comp_mgr->AddObject(&command_provider);

    manager.Build();

    CHECK(cmd_mgr->create_container_calls >= 1);  // Should create App Menu
    CHECK(cmd_mgr->created_containers.contains(sss::dscore::constants::menubars::kMainMenubar));

    CHECK(menu_provider.contribute_calls == 1);
    CHECK(toolbar_provider.contribute_calls == 1);
    CHECK(command_provider.register_calls == 1);
    CHECK(cmd_mgr->retranslate_calls == 1);

    comp_mgr->RemoveObject(&menu_provider);
    comp_mgr->RemoveObject(&toolbar_provider);
    comp_mgr->RemoveObject(&command_provider);
  }
}

#include "test_menu_and_toolbar_manager.moc"
