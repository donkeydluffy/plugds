#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief Theme Service Interface
 * Responsible for managing global QSS stylesheets, palette definitions, and font configurations.
 */
class DS_CORE_DLLSPEC IThemeService : public QObject {
  Q_OBJECT

 public:
  virtual ~IThemeService() = default;

  /**
   * @brief Load a theme by ID.
   * Reads the corresponding QSS file and applies it to qApp.
   *
   * @param theme_id Theme identifier (e.g., "dark", "light").
   */
  virtual auto LoadTheme(const QString& theme_id) -> void = 0;

  /**
   * @brief Get a semantic color defined by the current theme.
   * Allows components to fetch colors semantically rather than hardcoding HEX values.
   *
   * @param role Color role (e.g., "primary", "error", "background", "text_focus").
   * @returns QColor The corresponding color object.
   */
  [[nodiscard]] virtual auto GetColor(const QString& role) const -> QColor = 0;

  /**
   * @brief Get a standard font defined by the current theme.
   *
   * @param role Font role (e.g., "h1", "body", "code", "small").
   * @returns QFont The corresponding font object.
   */
  [[nodiscard]] virtual auto GetFont(const QString& role) const -> QFont = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IThemeService, "sss.dscore.IThemeService/1.0.0")
