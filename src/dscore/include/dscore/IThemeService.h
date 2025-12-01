#pragma once

#include <QColor>
#include <QObject>

#include "dscore/CoreSpec.h"
#include "dscore/Theme.h"

namespace sss::dscore {

/**
 * @brief Theme Service Interface
 * Responsible for managing global UI themes, including QPalette, QSS stylesheets, and semantic colors.
 */
class DS_CORE_DLLSPEC IThemeService : public QObject {
  Q_OBJECT

 public:
  ~IThemeService() override = default;

  /**
   * @brief Load and apply a theme by ID.
   * Reads the corresponding theme configuration (INI) and QSS template,
   * then applies the generated QPalette and QSS to qApp.
   *
   * @param theme_id Theme identifier (e.g., "dark", "light").
   */
  virtual auto LoadTheme(const QString& theme_id) -> void = 0;

  /**
   * @brief Get the current active Theme object.
   * Provides access to the full theme data for custom painting or advanced queries.
   *
   * @returns const Theme* A pointer to the current theme object.
   */
  [[nodiscard]] virtual auto Theme() const -> const Theme* = 0;

  /**
   * @brief Get a semantic color defined by the current theme.
   * Allows components to fetch colors semantically using strongly-typed roles.
   *
   * @param role Color role (e.g., Theme::BrandColor, Theme::PanelBackground).
   * @returns QColor The corresponding color object.
   */
  [[nodiscard]] virtual auto GetColor(Theme::ColorRole role) const -> QColor = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IThemeService, "sss.dscore.IThemeService/2.0.0")
