#include <doctest/doctest.h>

#include <QLabel>
#include <QSignalSpy>
#include <QStatusBar>

#include "dscore/StatusbarManager.h"

TEST_SUITE("StatusbarManager") {
  TEST_CASE("Status Message Operations") {
    QStatusBar status_bar;
    sss::dscore::StatusbarManager manager(&status_bar);

    SUBCASE("SetStatusMessage sets the message on QStatusBar") {
      QString message = "Test Status";
      manager.SetStatusMessage(message, 0);
      CHECK(status_bar.currentMessage() == message);
    }

    SUBCASE("ClearStatusMessage clears the message") {
      manager.SetStatusMessage("To be cleared", 0);
      CHECK(status_bar.currentMessage() == "To be cleared");

      manager.ClearStatusMessage();
      CHECK(status_bar.currentMessage().isEmpty());
    }
  }

  TEST_CASE("Permanent Widget Operations") {
    QStatusBar status_bar;
    sss::dscore::StatusbarManager manager(&status_bar);

    SUBCASE("AddPermanentWidget adds widget to QStatusBar") {
      auto* label = new QLabel("Permanent");
      // Note: 转移所有权到 QStatusBar
      manager.AddPermanentWidget(label, 0);

      // 检查父级关系
      CHECK(label->parent() == &status_bar);

      // 我们无法通过公共 API 轻松检查它是否是“永久的”，而不检查子布局
      // 但是确保它没有崩溃且父级是正确的。
    }

    SUBCASE("RemovePermanentWidget removes widget from QStatusBar") {
      auto* label = new QLabel("Permanent");
      manager.AddPermanentWidget(label, 0);

      // Remove it
      manager.RemovePermanentWidget(label);
    }
  }
}
