#pragma once

#include <memory>  // For std::unique_ptr

#include "dscore/IThemeService.h"
#include "dscore/Theme.h"

namespace sss::dscore {

class DS_CORE_DLLSPEC ThemeService : public sss::dscore::IThemeService {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IThemeService)

 public:
  ThemeService();
  ~ThemeService() override = default;

  // IThemeService implementation
  auto LoadTheme(const QString& theme_id) -> void override;
  [[nodiscard]] auto Theme() const -> const sss::dscore::Theme* override;
  [[nodiscard]] auto GetColor(Theme::ColorRole role) const -> QColor override;

 private:
  std::unique_ptr<sss::dscore::Theme> current_theme_;  // Stores the active theme data

  // Helper methods for parsing and applying
  auto applyPaletteToQapp() -> void;
  auto applyStyleSheetToQapp() -> void;

  // Helper to map string to QPalette::ColorRole (used during INI parsing)
  static auto stringToPaletteColorRole(const QString& str) -> QPalette::ColorRole;
  // Helper to map string to Theme::ColorRole (used during INI parsing)
  static auto stringToThemeColorRole(const QString& str) -> Theme::ColorRole;
  static auto themeColorRoleToString(Theme::ColorRole role) -> QString;
  static auto paletteColorRoleToString(QPalette::ColorRole role) -> QString;
};

}  // namespace sss::dscore
