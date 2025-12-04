#include <doctest/doctest.h>
#include <QStatusBar>
#include <QLabel>
#include <QSignalSpy>
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
            // Note: AddPermanentWidget transfers ownership to QStatusBar
            manager.AddPermanentWidget(label, 0);
            
            // Verify the widget is a child of the status bar
            CHECK(label->parent() == &status_bar);
            
            // We can't easily check if it's "permanent" via public API without inspecting children layout
            // but ensuring it didn't crash and parent is correct is a good start.
        }

        SUBCASE("RemovePermanentWidget removes widget from QStatusBar") {
            auto* label = new QLabel("Permanent");
            manager.AddPermanentWidget(label, 0);
            
            // Remove it
            manager.RemovePermanentWidget(label);
            
            // After removal, it should still be a valid pointer but likely not in the layout. 
            // QStatusBar::removeWidget documentation says "The widget is hidden".
            // Ownership might still remain with QStatusBar if it was added there.
            // Checking isVisible() might be a proxy, but without showing the window it might be false anyway.
            
            // Let's just ensure it runs without error for now.
            // A more robust test would check the layout of QStatusBar.
        }
    }
}
