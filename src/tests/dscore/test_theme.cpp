#include <doctest/doctest.h>
#include <dscore/Theme.h>

#include <QColor>
#include <QPalette>

TEST_SUITE("Theme") {
  TEST_CASE("Initialization") {
    sss::dscore::Theme theme("dark_test");
    CHECK(theme.Id() == "dark_test");
    CHECK(theme.StyleSheet().isEmpty());
  }

  TEST_CASE("Set and Get Color") {
    sss::dscore::Theme theme("test");
    QColor test_color = Qt::red;
    theme.SetColor(sss::dscore::Theme::kBrandColor, test_color);

    CHECK(theme.Color(sss::dscore::Theme::kBrandColor) == test_color);

    // 默认或未设置的颜色可能有特定的行为，但实现显示它使用 QVector<QColor> colors_。
    CHECK(theme.Color(sss::dscore::Theme::kCount) == QColor(Qt::magenta));
  }

  TEST_CASE("Set and Get Palette Color") {
    sss::dscore::Theme theme("test");
    QColor window_color = Qt::black;
    theme.SetPaletteColor(QPalette::Active, QPalette::Window, window_color);

    CHECK(theme.Palette().color(QPalette::Active, QPalette::Window) == window_color);
  }

  TEST_CASE("Set Style Sheet") {
    sss::dscore::Theme theme("test");
    QString qss = "QWidget { background: red; }";
    theme.SetStyleSheet(qss);
    CHECK(theme.StyleSheet() == qss);
  }
}
