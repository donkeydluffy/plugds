#include <doctest/doctest.h>
#include <dscore/CoreConstants.h>

#include <QSignalSpy>

#include "ContextManager.h"

TEST_SUITE("ContextManager") {
  TEST_CASE("Initialization") {
    sss::dscore::ContextManager mgr;
    CHECK(mgr.GetActiveContexts().size() == 1);
    CHECK(mgr.GetActiveContexts().contains(sss::dscore::kGlobalContext));
    CHECK(mgr.Context() == sss::dscore::kGlobalContext);
  }

  TEST_CASE("RegisterContext") {
    sss::dscore::ContextManager mgr;
    QString ctx_name = "Editor";
    int id = mgr.RegisterContext(ctx_name);
    CHECK(id != 0);
    CHECK(id != sss::dscore::kGlobalContext);

    // 注册相同名称返回相同ID
    CHECK(mgr.RegisterContext(ctx_name) == id);

    // 按名称查找
    CHECK(mgr.Context(ctx_name) == id);
    CHECK(mgr.Context("NonExistent") == 0);
  }

  TEST_CASE("SetContext") {
    sss::dscore::ContextManager mgr;
    int ctx1 = mgr.RegisterContext("Ctx1");

    QSignalSpy spy(&mgr, &sss::dscore::ContextManager::ContextChanged);

    mgr.SetContext(ctx1);

    CHECK(mgr.Context() == ctx1);
    CHECK(mgr.GetActiveContexts().size() == 2);  // 全局 + Ctx1
    CHECK(mgr.GetActiveContexts().contains(ctx1));
    CHECK(spy.count() == 1);
    QList<QVariant> args = spy.takeFirst();
    CHECK(args.at(0).toInt() == ctx1);  // 新上下文
  }

  TEST_CASE("Add/Remove Active Context") {
    sss::dscore::ContextManager mgr;
    int ctx1 = mgr.RegisterContext("Ctx1");
    int ctx2 = mgr.RegisterContext("Ctx2");

    mgr.SetContext(ctx1);
    CHECK(mgr.Context() == ctx1);

    mgr.AddActiveContext(ctx2);
    CHECK(mgr.Context() == ctx2);  // 最后添加的是主要的
    CHECK(mgr.GetActiveContexts().contains(ctx1));
    CHECK(mgr.GetActiveContexts().contains(ctx2));

    mgr.RemoveActiveContext(ctx2);
    CHECK(mgr.Context() == ctx1);
    CHECK_FALSE(mgr.GetActiveContexts().contains(ctx2));
  }

  TEST_CASE("ActivateMode (Save and Restore sub-contexts)") {
    sss::dscore::ContextManager mgr;
    int mode_a = mgr.RegisterContext("ModeA");
    int mode_b = mgr.RegisterContext("ModeB");
    int tool_a1 = mgr.RegisterContext("ToolA1");

    // 在ModeA中开始
    mgr.ActivateMode(mode_a);
    CHECK(mgr.Context() == mode_a);

    // 添加工具上下文
    mgr.AddActiveContext(tool_a1);
    CHECK(mgr.GetActiveContexts().contains(tool_a1));

    // 切换到ModeB
    mgr.ActivateMode(mode_b);
    CHECK(mgr.Context() == mode_b);
    CHECK_FALSE(mgr.GetActiveContexts().contains(tool_a1));  // 应该被清除

    // 切换回ModeA
    mgr.ActivateMode(mode_a);
    CHECK(mgr.Context() == tool_a1);  // 应该恢复toolA1，因为它最后是active的（或至少存在）
    // Context()返回active_contexts_.last()。实现会附加恢复的上下文。
    // 如果toolA1在切换前在active_contexts_中，它被保存了。
    // 恢复逻辑：迭代保存的上下文并附加。

    CHECK(mgr.GetActiveContexts().contains(tool_a1));
  }
}
