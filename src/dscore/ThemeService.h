#pragma once
#include <QMap>

#include "dscore/IThemeService.h"

namespace sss::dscore {

class ThemeService : public sss::dscore::IThemeService {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IThemeService)

 public:
  ThemeService();
  ~ThemeService() override = default;

  auto LoadTheme(const QString& theme_id) -> void override;
  [[nodiscard]] auto GetColor(const QString& role) const -> QColor override;
  [[nodiscard]] auto GetFont(const QString& role) const -> QFont override;

 private:
  QMap<QString, QColor> current_colors_;
  QMap<QString, QFont> current_fonts_;
};

}  // namespace sss::dscore
