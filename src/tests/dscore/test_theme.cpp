#include <doctest/doctest.h>
#include <dscore/Theme.h>
#include <QPalette>
#include <QColor>

TEST_SUITE("Theme") {
  TEST_CASE("Initialization") {
    sss::dscore::Theme theme("dark_test");
    CHECK(theme.Id() == "dark_test");
    CHECK(theme.StyleSheet().isEmpty());
  }

  TEST_CASE("Set and Get Color") {
    sss::dscore::Theme theme("test");
    QColor testColor = Qt::red;
    theme.SetColor(sss::dscore::Theme::kBrandColor, testColor);
    
    CHECK(theme.Color(sss::dscore::Theme::kBrandColor) == testColor);
    
    // Default or unset colors might behave specifically, but implementation shows it uses QVector<QColor> colors_.
    // Typically initialized to default QColor() which is invalid.
    // Implementation initializes to Qt::magenta, which is VALID.
    // And out of bounds also returns Qt::magenta.
    CHECK(theme.Color(sss::dscore::Theme::kCount) == QColor(Qt::magenta)); 
  }

  TEST_CASE("Set and Get Palette Color") {
    sss::dscore::Theme theme("test");
    QColor windowColor = Qt::black;
    theme.SetPaletteColor(QPalette::Active, QPalette::Window, windowColor);
    
    CHECK(theme.Palette().color(QPalette::Active, QPalette::Window) == windowColor);
    
    // Check if other groups are unaffected unless set
    // QPalette defaults might be system dependent, but we set one specific role/group.
  }

  TEST_CASE("Set Style Sheet") {
    sss::dscore::Theme theme("test");
    QString qss = "QWidget { background: red; }";
    theme.SetStyleSheet(qss);
    CHECK(theme.StyleSheet() == qss);
  }
}
