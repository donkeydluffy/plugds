#include <doctest/doctest.h>
#include "ContextManager.h"
#include <dscore/CoreConstants.h>
#include <QSignalSpy>

TEST_SUITE("ContextManager") {
  TEST_CASE("Initialization") {
    sss::dscore::ContextManager mgr;
    CHECK(mgr.GetActiveContexts().size() == 1);
    CHECK(mgr.GetActiveContexts().contains(sss::dscore::kGlobalContext));
    CHECK(mgr.Context() == sss::dscore::kGlobalContext);
  }

  TEST_CASE("RegisterContext") {
    sss::dscore::ContextManager mgr;
    QString ctxName = "Editor";
    int id = mgr.RegisterContext(ctxName);
    CHECK(id != 0);
    CHECK(id != sss::dscore::kGlobalContext);
    
    // Registering same name returns same ID
    CHECK(mgr.RegisterContext(ctxName) == id);
    
    // Finding by name
    CHECK(mgr.Context(ctxName) == id);
    CHECK(mgr.Context("NonExistent") == 0);
  }

  TEST_CASE("SetContext") {
    sss::dscore::ContextManager mgr;
    int ctx1 = mgr.RegisterContext("Ctx1");
    
    QSignalSpy spy(&mgr, &sss::dscore::ContextManager::ContextChanged);
    
    mgr.SetContext(ctx1);
    
    CHECK(mgr.Context() == ctx1);
    CHECK(mgr.GetActiveContexts().size() == 2); // Global + Ctx1
    CHECK(mgr.GetActiveContexts().contains(ctx1));
    CHECK(spy.count() == 1);
    QList<QVariant> args = spy.takeFirst();
    CHECK(args.at(0).toInt() == ctx1); // New context
  }

  TEST_CASE("Add/Remove Active Context") {
    sss::dscore::ContextManager mgr;
    int ctx1 = mgr.RegisterContext("Ctx1");
    int ctx2 = mgr.RegisterContext("Ctx2");
    
    mgr.SetContext(ctx1);
    CHECK(mgr.Context() == ctx1);
    
    mgr.AddActiveContext(ctx2);
    CHECK(mgr.Context() == ctx2); // Last added is primary
    CHECK(mgr.GetActiveContexts().contains(ctx1));
    CHECK(mgr.GetActiveContexts().contains(ctx2));
    
    mgr.RemoveActiveContext(ctx2);
    CHECK(mgr.Context() == ctx1);
    CHECK_FALSE(mgr.GetActiveContexts().contains(ctx2));
  }

  TEST_CASE("ActivateMode (Save and Restore sub-contexts)") {
    sss::dscore::ContextManager mgr;
    int modeA = mgr.RegisterContext("ModeA");
    int modeB = mgr.RegisterContext("ModeB");
    int toolA1 = mgr.RegisterContext("ToolA1");
    
    // Start in ModeA
    mgr.ActivateMode(modeA);
    CHECK(mgr.Context() == modeA);
    
    // Add a tool context
    mgr.AddActiveContext(toolA1);
    CHECK(mgr.GetActiveContexts().contains(toolA1));
    
    // Switch to ModeB
    mgr.ActivateMode(modeB);
    CHECK(mgr.Context() == modeB);
    CHECK_FALSE(mgr.GetActiveContexts().contains(toolA1)); // Should be cleared
    
    // Switch back to ModeA
    mgr.ActivateMode(modeA);
    CHECK(mgr.Context() == toolA1); // Should restore toolA1 as it was last active (or at least present)
    // Context() returns active_contexts_.last(). The implementation appends restored contexts.
    // If toolA1 was in active_contexts_ before switch, it was saved.
    // Restore logic: iterates saved contexts and appends.
    
    CHECK(mgr.GetActiveContexts().contains(toolA1));
  }
}
