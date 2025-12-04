#include <doctest/doctest.h>
#include "ThemeService.h"
#include <dscore/Theme.h>
#include <QApplication>
#include <QColor>

TEST_SUITE("ThemeService") {
  TEST_CASE("Initialization") {
    // ThemeService constructor modifies QApplication style and font.
    // We assume QApplication is already created in main.cpp.
    sss::dscore::ThemeService service;
    CHECK(service.Theme() != nullptr);
    CHECK(service.Theme()->Id() == "default");
  }

  TEST_CASE("GetColor Fallback") {
    sss::dscore::ThemeService service;
    // Default theme might not have all colors set, or "default" is just a name.
    // But GetColor should return something (fallback logic in implementation says magenta if theme invalid, but theme is valid here).
    // If role is not set in theme, Theme::Color returns invalid QColor? 
    // Let's check implementation of Theme::Color in Theme.cpp if possible, 
    // but based on header it uses QVector.
    
    // If we request a color that hasn't been loaded (since we didn't call LoadTheme with a valid file),
    // we expect invalid color or default.
    QColor color = service.GetColor(sss::dscore::Theme::kBrandColor);
    CHECK(color.isValid());
    // Without LoadTheme("..."), the theme is empty.
    // Let's see what happens.
  }

  TEST_CASE("LoadTheme Invalid") {
    sss::dscore::ThemeService service;
    // Try loading non-existent theme
    service.LoadTheme("non_existent_theme");
    // Should handle gracefully, logs error. 
    // Theme should probably remain default or be updated to empty one with that ID.
    // Implementation makes new_theme and if config doesn't exist returns.
    CHECK(service.Theme()->Id() == "default"); // Should stay default
  }
  
  // Cannot easily test LoadTheme("valid") without mocking resources or file system.
}
