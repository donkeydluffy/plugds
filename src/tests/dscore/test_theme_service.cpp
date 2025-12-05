#include <doctest/doctest.h>
#include <dscore/Theme.h>

#include <QApplication>
#include <QColor>

#include "ThemeService.h"

TEST_SUITE("ThemeService") {
  TEST_CASE("Initialization") {
    // QApplication 已经在 main.cpp 中创建。
    sss::dscore::ThemeService service;
    CHECK(service.Theme() != nullptr);
    CHECK(service.Theme()->Id() == "default");
  }

  TEST_CASE("GetColor Fallback") {
    sss::dscore::ThemeService service;

    QColor color = service.GetColor(sss::dscore::Theme::kBrandColor);
    CHECK(color.isValid());
  }

  TEST_CASE("LoadTheme Invalid") {
    sss::dscore::ThemeService service;
    // 不存在的主题ID
    service.LoadTheme("non_existent_theme");
    CHECK(service.Theme()->Id() == "default");  // Should stay default
  }
}
