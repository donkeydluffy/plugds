#include <doctest/doctest.h>
#include <dscore/ModeManager.h>
#include <dscore/IMode.h>
#include <dscore/IWorkbench.h>
#include <dscore/ContextManager.h>
#include <extsystem/IComponentManager.h>
#include <QSignalSpy>
#include <QList>

// Mock IMode
class MockMode : public sss::dscore::IMode {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IMode)
 public:
  MockMode(QString id, QString title, int ctxId) : id_(id), title_(title), ctxId_(ctxId) {}
  
  QString Id() const override { return id_; }
  QString Title() const override { return title_; }
  QIcon Icon() const override { return QIcon(); }
  int ContextId() const override { return ctxId_; }
  int Priority() const override { return 0; }
  void Activate() override { active_ = true; }
  void Deactivate() override { active_ = false; }
  
  bool IsActive() const { return active_; }

 private:
  QString id_;
  QString title_;
  int ctxId_;
  bool active_ = false;
};

// Mock Workbench
class MockWorkbench : public QObject, public sss::dscore::IWorkbench {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IWorkbench)
 public:
  // IWorkbench implementation
  void AddSidePanel(const QString& id, QWidget* panel, const QString& title, const QIcon& icon) override {
      (void)id; (void)panel; (void)title; (void)icon;
  }
  void SetBackgroundWidget(QWidget* widget) override {
      (void)widget;
  }
  void AddSqueezeWidget(sss::dscore::SqueezeSide side, QWidget* widget, int priority,
                        const QList<int>& visible_contexts, const QList<int>& enable_contexts) override {
      (void)side; (void)widget; (void)priority; (void)visible_contexts; (void)enable_contexts;
  }
  void AddOverlayWidget(sss::dscore::OverlayZone zone, QWidget* widget, int priority,
                        const QList<int>& visible_contexts, const QList<int>& enable_contexts) override {
      (void)zone; (void)widget; (void)priority; (void)visible_contexts; (void)enable_contexts;
  }
  void ShowNotification(const QString& message, int duration_ms) override {
      (void)message; (void)duration_ms;
  }
  void Clear() override {}
  void AddModeButton(const QString& id, const QString& title, const QIcon& icon) override {
      (void)id; (void)title; (void)icon;
  }
  void SetActiveModeButton(const QString& id) override {
      (void)id;
  }
  void SetModeSwitchCallback(std::function<void(const QString&)> callback) override {
      (void)callback;
  }
};

struct ModeManagerFixture {
  sss::extsystem::IComponentManager* comp_mgr = nullptr;
  sss::dscore::ContextManager* context_mgr = nullptr;
  sss::dscore::ModeManager* mode_mgr = nullptr;
  MockWorkbench* mock_workbench = nullptr;

  ModeManagerFixture() {
    comp_mgr = sss::extsystem::IComponentManager::GetInstance();
    // Clear existing
    auto all = comp_mgr->AllObjects();
    for(auto* o : all) comp_mgr->RemoveObject(o);

    context_mgr = new sss::dscore::ContextManager();
    comp_mgr->AddObject(context_mgr);
    
    mock_workbench = new MockWorkbench();
    comp_mgr->AddObject(mock_workbench);

    mode_mgr = new sss::dscore::ModeManager();
    mode_mgr->SetGlobalWorkbench(mock_workbench);
    // ModeManager usually is not strictly required to be in ComponentManager for its own methods, 
    // but we might add it.
  }

  ~ModeManagerFixture() {
    delete mode_mgr;
    comp_mgr->RemoveObject(context_mgr);
    delete context_mgr;
    
    comp_mgr->RemoveObject(mock_workbench);
    delete mock_workbench;
  }
};

TEST_SUITE("ModeManager") {
  TEST_CASE_FIXTURE(ModeManagerFixture, "Add and Activate Mode") {
    MockMode* mode1 = new MockMode("mode1", "Mode 1", 10);
    MockMode* mode2 = new MockMode("mode2", "Mode 2", 20);

    QSignalSpy spyAdded(mode_mgr, &sss::dscore::ModeManager::ModeAdded);
    QSignalSpy spyChanged(mode_mgr, &sss::dscore::ModeManager::ModeChanged);

    mode_mgr->AddMode(mode1);
    CHECK(mode_mgr->Modes().size() == 1);
    CHECK(spyAdded.count() == 1);

    mode_mgr->AddMode(mode2);
    CHECK(mode_mgr->Modes().size() == 2);
    CHECK(spyAdded.count() == 2);

    // Activate Mode 1
    mode_mgr->ActivateMode("mode1");
    CHECK(mode_mgr->ActiveMode() == mode1);
    CHECK(mode1->IsActive());
    CHECK_FALSE(mode2->IsActive());
    CHECK(spyChanged.count() == 1);
    
    // Verify Context Switch
    CHECK(context_mgr->Context() == 10); // Mode 1 Context ID

    // Activate Mode 2
    mode_mgr->ActivateMode("mode2");
    CHECK(mode_mgr->ActiveMode() == mode2);
    CHECK_FALSE(mode1->IsActive());
    CHECK(mode2->IsActive());
    CHECK(spyChanged.count() == 2);
    
    CHECK(context_mgr->Context() == 20); // Mode 2 Context ID

    delete mode1;
    delete mode2;
  }
  
  TEST_CASE_FIXTURE(ModeManagerFixture, "Activate Unknown Mode") {
    mode_mgr->ActivateMode("unknown");
    CHECK(mode_mgr->ActiveMode() == nullptr);
  }
}

#include "test_mode_manager.moc"