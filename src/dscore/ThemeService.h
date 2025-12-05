#pragma once

#include <QIcon>
#include <QMap>
#include <memory>

#include "dscore/IThemeService.h"
#include "dscore/Theme.h"

namespace sss::dscore {

class DS_CORE_DLLSPEC ThemeService : public sss::dscore::IThemeService {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IThemeService)

 public:
  ThemeService();
  ~ThemeService() override = default;

  // IThemeService接口实现
  auto LoadTheme(const QString& theme_id) -> void override;
  [[nodiscard]] auto Theme() const -> const sss::dscore::Theme* override;
  [[nodiscard]] auto GetColor(Theme::ColorRole role) const -> QColor override;
  [[nodiscard]] auto GetIcon(const QString& base_path, const QString& icon_name) const -> QIcon override;

 private:
  std::unique_ptr<sss::dscore::Theme> current_theme_;  // 存储当前活动的主题数据

  // 用于解析和应用的帮助方法
  auto applyPaletteToQapp() -> void;
  auto applyStyleSheetToQapp() -> void;

  // 将字符串映射到 QPalette::ColorRole 的辅助函数（用于 INI 解析）
  static auto stringToPaletteColorRole(const QString& str) -> QPalette::ColorRole;
  // 将字符串映射到 Theme::ColorRole 的辅助函数（用于 INI 解析）
  static auto stringToThemeColorRole(const QString& str) -> Theme::ColorRole;
  static auto themeColorRoleToString(Theme::ColorRole role) -> QString;
  static auto paletteColorRoleToString(QPalette::ColorRole role) -> QString;

  // 图标缓存：键 = "theme_type|base_path|icon_name"
  mutable QMap<QString, QIcon> icon_cache_;
};

}  // namespace sss::dscore
