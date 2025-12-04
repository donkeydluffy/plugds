#include "ThemeService.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QFontInfo>
#include <QMetaEnum>
#include <QProxyStyle>
#include <QSettings>
#include <QStyleFactory>
#include <QWidget>
#include <algorithm>
#include <utility>

namespace sss::dscore {

// 辅助函数：将字符串映射到QPalette::ColorRole
auto ThemeService::stringToPaletteColorRole(const QString& str) -> QPalette::ColorRole {
  static const QMetaObject& mo = QPalette::staticMetaObject;
  static int index = mo.indexOfEnumerator("ColorRole");
  static QMetaEnum meta_enum = mo.enumerator(index);

  // 使用keyToValue进行字符串到枚举值的直接转换
  int val = meta_enum.keyToValue(str.toLatin1().constData());
  if (val == -1) {
    qWarning() << "ThemeService：未知的 QPalette::ColorRole 字符串：" << str;
    return QPalette::NColorRoles;  // 返回无效角色
  }
  return static_cast<QPalette::ColorRole>(val);
}

// 辅助函数：将字符串映射到Theme::ColorRole
auto ThemeService::stringToThemeColorRole(const QString& str) -> Theme::ColorRole {
  // 此映射存储QString（如"THEME_COLOR_BrandColor"）到Theme::ColorRole的对应关系
  static const QMap<QString, Theme::ColorRole> kStringToRoleMap = {
      {"THEME_COLOR_BrandColor", Theme::kBrandColor},
      {"THEME_COLOR_BrandColorHover", Theme::kBrandColorHover},
      {"THEME_COLOR_BrandColorPressed", Theme::kBrandColorPressed},
      {"THEME_COLOR_ErrorColor", Theme::kErrorColor},
      {"THEME_COLOR_WarningColor", Theme::kWarningColor},
      {"THEME_COLOR_SuccessColor", Theme::kSuccessColor},
      {"THEME_COLOR_InfoColor", Theme::kInfoColor},
      {"THEME_COLOR_PanelBackground", Theme::kPanelBackground},
      {"THEME_COLOR_PanelBorder", Theme::kPanelBorder},
      {"THEME_COLOR_EditorBackground", Theme::kEditorBackground},
      {"THEME_COLOR_EditorTextColor", Theme::kEditorTextColor},
      {"THEME_COLOR_EditorLineNumber", Theme::kEditorLineNumber},
      {"THEME_COLOR_EditorSelection", Theme::kEditorSelection},
      {"THEME_COLOR_ScrollBarBackground", Theme::kScrollBarBackground},
      {"THEME_COLOR_ScrollBarHandle", Theme::kScrollBarHandle},
      {"THEME_COLOR_ScrollBarHandleHover", Theme::kScrollBarHandleHover},
      {"THEME_COLOR_MenuBackground", Theme::kMenuBackground},
      {"THEME_COLOR_MenuBorder", Theme::kMenuBorder},
      {"THEME_COLOR_MenuItemHover", Theme::kMenuItemHover},
      {"THEME_COLOR_MenuItemText", Theme::kMenuItemText},
      {"THEME_COLOR_TextPrimary", Theme::kTextPrimary},
      {"THEME_COLOR_TextSecondary", Theme::kTextSecondary},
      {"THEME_COLOR_TextDisabled", Theme::kTextDisabled},
      {"THEME_COLOR_OverlayBackground", Theme::kOverlayBackground},
      {"THEME_COLOR_OverlayText", Theme::kOverlayText},
      {"THEME_COLOR_OverlayAccent", Theme::kOverlayAccent}};
  Theme::ColorRole role = kStringToRoleMap.value(str, Theme::kCount);  // 正确：使用.value()和QString键
  if (role == Theme::kCount) {
    qWarning() << "ThemeService：未知的 Theme::ColorRole 查找字符串：" << str;
  }
  return role;
}

// 辅助函数：将Theme::ColorRole映射到字符串用于QSS替换
auto ThemeService::themeColorRoleToString(Theme::ColorRole role) -> QString {
  // 此映射存储Theme::ColorRole到QString（如"THEME_COLOR_BrandColor"）的对应关系
  static const QMap<Theme::ColorRole, QString> kRoleToStringMap = {
      {Theme::kBrandColor, "THEME_COLOR_BrandColor"},
      {Theme::kBrandColorHover, "THEME_COLOR_BrandColorHover"},
      {Theme::kBrandColorPressed, "THEME_COLOR_BrandColorPressed"},
      {Theme::kErrorColor, "THEME_COLOR_ErrorColor"},
      {Theme::kWarningColor, "THEME_COLOR_WarningColor"},
      {Theme::kSuccessColor, "THEME_COLOR_SuccessColor"},
      {Theme::kInfoColor, "THEME_COLOR_InfoColor"},
      {Theme::kPanelBackground, "THEME_COLOR_PanelBackground"},
      {Theme::kPanelBorder, "THEME_COLOR_PanelBorder"},
      {Theme::kEditorBackground, "THEME_COLOR_EditorBackground"},
      {Theme::kEditorTextColor, "THEME_COLOR_EditorTextColor"},
      {Theme::kEditorLineNumber, "THEME_COLOR_EditorLineNumber"},
      {Theme::kEditorSelection, "THEME_COLOR_EditorSelection"},
      {Theme::kScrollBarBackground, "THEME_COLOR_ScrollBarBackground"},
      {Theme::kScrollBarHandle, "THEME_COLOR_ScrollBarHandle"},
      {Theme::kScrollBarHandleHover, "THEME_COLOR_ScrollBarHandleHover"},
      {Theme::kMenuBackground, "THEME_COLOR_MenuBackground"},
      {Theme::kMenuBorder, "THEME_COLOR_MenuBorder"},
      {Theme::kMenuItemHover, "THEME_COLOR_MenuItemHover"},
      {Theme::kMenuItemText, "THEME_COLOR_MenuItemText"},
      {Theme::kTextPrimary, "THEME_COLOR_TextPrimary"},
      {Theme::kTextSecondary, "THEME_COLOR_TextSecondary"},
      {Theme::kTextDisabled, "THEME_COLOR_TextDisabled"},
      {Theme::kOverlayBackground, "THEME_COLOR_OverlayBackground"},
      {Theme::kOverlayText, "THEME_COLOR_OverlayText"},
      {Theme::kOverlayAccent, "THEME_COLOR_OverlayAccent"}};
  return kRoleToStringMap.value(role, "UNKNOWN_COLOR_ROLE");
}

// 辅助函数：将QPalette::ColorRole映射到字符串用于QSS替换
auto ThemeService::paletteColorRoleToString(QPalette::ColorRole role) -> QString {
  static const QMap<QPalette::ColorRole, QString> kMap = {
      {QPalette::Window, "PALETTE_Window"},
      {QPalette::WindowText, "PALETTE_WindowText"},
      {QPalette::Base, "PALETTE_Base"},
      {QPalette::Text, "PALETTE_Text"},
      {QPalette::Button, "PALETTE_Button"},
      {QPalette::ButtonText, "PALETTE_ButtonText"},
      {QPalette::Highlight, "PALETTE_Highlight"},
      {QPalette::HighlightedText, "PALETTE_HighlightedText"},
      {QPalette::AlternateBase, "PALETTE_AlternateBase"}  // 为表格添加
  };
  return kMap.value(role, "UNKNOWN_PALETTE_ROLE");
}

ThemeService::ThemeService() {
  // 强制使用Fusion风格以获得一致的跨平台主题支持
  // Fusion比原生风格（Windows/GTK）更好地尊重QPalette
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  // 设置一致的应用程序字体，具有通用回退
  QFont font("DejaVu Sans", 9);  // 使用DejaVu Sans字体，Linux系统上常见
  if (!font.exactMatch()) {
    font = QFont("sans-serif", 9);  // 通用sans-serif回退字体
  }
  QApplication::setFont(font);

  // 初始化默认主题实例，将在首次加载时替换
  current_theme_ = std::make_unique<sss::dscore::Theme>("default");
}

auto ThemeService::LoadTheme(const QString& theme_id) -> void {
  SPDLOG_INFO("\n=== ThemeService: Starting theme load for ID: {} ===", theme_id.toStdString());
  QString config_path = QString(":/dscore/resources/themes/%1.ini").arg(theme_id);

  if (!QFile::exists(config_path)) {
    SPDLOG_ERROR("ThemeService: CRITICAL ERROR - INI file does not exist at path: {}", config_path.toStdString());
    return;
  }

  QSettings settings(config_path, QSettings::IniFormat);
  if (settings.status() != QSettings::NoError) {
    SPDLOG_ERROR("ThemeService: QSettings failed to parse INI file: {} Status: {}", config_path.toStdString(),
                 settings.status());
    return;
  }

  SPDLOG_INFO("ThemeService: Loading INI file: {}", config_path.toStdString());

  auto new_theme = std::make_unique<sss::dscore::Theme>(theme_id);

  // 1. 解析 [Palette] 组 -> QPalette
  settings.beginGroup("Palette");
  QStringList palette_keys = settings.childKeys();
  qDebug() << "ThemeService：找到" << palette_keys.size() << "个调色板条目。";

  if (palette_keys.isEmpty()) {
    qWarning() << "ThemeService：[Palette] 部分为空或缺失！";
  }

  for (const auto& key : settings.childKeys()) {
    QPalette::ColorRole role = stringToPaletteColorRole(key);
    QColor color(settings.value(key).toString());

    if (role != QPalette::NColorRoles && color.isValid()) {
      // 设置所有状态 (Active, Inactive, Disabled)
      new_theme->SetPaletteColor(QPalette::Active, role, color);
      new_theme->SetPaletteColor(QPalette::Inactive, role, color);
      new_theme->SetPaletteColor(QPalette::Disabled, role, color.lighter(60));
    } else {
      qWarning() << "ThemeService: Invalid Palette entry:" << key << "=" << settings.value(key).toString();
    }
  }
  settings.endGroup();

  // 2. 解析 [Colors] 组 -> Theme::ColorRole
  settings.beginGroup("Colors");
  QStringList color_keys = settings.childKeys();
  SPDLOG_INFO("ThemeService: Found {} custom color entries.", color_keys.size());

  for (const auto& key : settings.childKeys()) {
    Theme::ColorRole role = stringToThemeColorRole(key);
    QString value = settings.value(key).toString();
    QColor color(value);

    if (role != Theme::kCount && color.isValid()) {
      new_theme->SetColor(role, color);
    } else {
      qWarning() << "ThemeService: Invalid Colors entry:" << key << "=" << value;
    }
  }
  settings.endGroup();

  // 3. QSS 模板渲染
  QString qss_template_path = ":/dscore/resources/themes/base.qss";
  QFile qss_file(qss_template_path);
  QString generated_qss;

  if (qss_file.open(QFile::ReadOnly | QFile::Text)) {
    SPDLOG_INFO("ThemeService: QSS template loaded. Size: {}", qss_file.size());
    generated_qss = qss_file.readAll();
    qss_file.close();

    // 3.1 替换 Theme::ColorRole 变量 (按长度降序替换，避免短名称被错误替换)
    QVector<std::pair<QString, Theme::ColorRole>> sorted_theme_roles;
    for (int i = 0; i < Theme::kCount; ++i) {
      auto role = static_cast<Theme::ColorRole>(i);
      sorted_theme_roles.append({themeColorRoleToString(role), role});
    }
    std::sort(sorted_theme_roles.begin(), sorted_theme_roles.end(),
              [](const auto& a, const auto& b) { return a.first.length() > b.first.length(); });

    for (const auto& pair : sorted_theme_roles) {
      QString role_str = pair.first;
      QColor color_val = new_theme->Color(pair.second);
      QString color_name = color_val.name();
      QString replace_marker = QString("@@%1@@").arg(role_str);
      generated_qss.replace(replace_marker, color_name);
    }

    // 3.2 替换 QPalette 常用变量 (也使用 @@ 格式)
    QVector<std::pair<QString, QPalette::ColorRole>> sorted_palette_roles;
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::Window), QPalette::Window});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::WindowText), QPalette::WindowText});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::Base), QPalette::Base});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::Text), QPalette::Text});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::Button), QPalette::Button});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::ButtonText), QPalette::ButtonText});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::Highlight), QPalette::Highlight});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::HighlightedText), QPalette::HighlightedText});
    sorted_palette_roles.append({paletteColorRoleToString(QPalette::AlternateBase), QPalette::AlternateBase});

    std::sort(sorted_palette_roles.begin(), sorted_palette_roles.end(),
              [](const auto& a, const auto& b) { return a.first.length() > b.first.length(); });

    for (const auto& pair : sorted_palette_roles) {
      QString role_str = pair.first;
      QColor color_val = new_theme->Palette().color(pair.second);
      generated_qss.replace(QString("@@%1@@").arg(role_str), color_val.name());
    }

    new_theme->SetStyleSheet(generated_qss);
    SPDLOG_INFO("ThemeService: QSS generated successfully. Size: {}", generated_qss.length());

  } else {
    SPDLOG_ERROR("ThemeService: Failed to open QSS template file: {}", qss_template_path.toStdString());
  }

  current_theme_ = std::move(new_theme);

  // 应用新的主题
  SPDLOG_INFO("\nThemeService: Applying palette to application...");
  applyPaletteToQapp();

  SPDLOG_INFO("ThemeService: Applying stylesheet to application...");
  applyStyleSheetToQapp();

  SPDLOG_INFO("=== ThemeService: Theme {} applied completely ===\n", theme_id.toStdString());

  emit ThemeChanged(theme_id);
}

auto ThemeService::Theme() const -> const sss::dscore::Theme* { return current_theme_.get(); }

auto ThemeService::GetColor(Theme::ColorRole role) const -> QColor {
  // 解引用前确保current_theme_有效
  if (current_theme_) {
    return current_theme_->Color(role);
  }
  return {Qt::magenta};  // 回退颜色
}

auto ThemeService::GetIcon(const QString& base_path, const QString& icon_name) const -> QIcon {
  QString theme_type = "light";  // 默认

  if (current_theme_ && current_theme_->Id().contains("dark", Qt::CaseInsensitive)) {
    theme_type = "dark";
  }

  // 生成唯一的缓存键

  // 格式："dark|:/path/to/resources|filename.svg"

  QString cache_key = theme_type + "|" + base_path + "|" + icon_name;

  // 首先检查缓存

  if (icon_cache_.contains(cache_key)) {
    return icon_cache_.value(cache_key);
  }

  // 构建路径

  QString path = base_path;

  if (!path.endsWith('/')) {
    path += '/';
  }

  path += theme_type + "/" + icon_name;

  QIcon icon;

  if (QFile::exists(path)) {
    icon = QIcon(path);

  } else {
    // 如需要可在此添加回退逻辑

    qWarning() << "ThemeService: Icon not found at" << path;
  }

  // 存入缓存（即使为空，以避免重复的文件系统检查）

  icon_cache_.insert(cache_key, icon);

  return icon;
}

auto ThemeService::applyPaletteToQapp() -> void {
  if (current_theme_) {
    QPalette new_palette = current_theme_->Palette();

    qApp->setPalette(new_palette);

    // 注意：qApp上的setPalette通常会传播，出于性能考虑移除了显式的小部件循环

    // 除非特定小部件阻止调色板传播。
  }
}

auto ThemeService::applyStyleSheetToQapp() -> void {
  if (current_theme_) {
    const QString& stylesheet = current_theme_->StyleSheet();

    // 性能优化：禁用更新以防止闪烁和中间重绘

    // 在繁重的样式表解析和应用期间。

    for (QWidget* widget : QApplication::topLevelWidgets()) {
      if (widget != nullptr) widget->setUpdatesEnabled(false);
    }

    // 应用样式表

    // 这是繁重的操作。

    qApp->setStyleSheet(stylesheet);

    // 重新启用更新

    for (QWidget* widget : QApplication::topLevelWidgets()) {
      if (widget != nullptr) widget->setUpdatesEnabled(true);
    }

    // 注意：移除了手动递归的unpolish/polish循环。

    // qApp->setStyleSheet()自动触发全局更新。

    // 手动循环的时间复杂度为O(N)，其中N是总小部件数，会导致延迟。

    // 如果特定小部件未能更新，它们应该监听QEvent::StyleChange。
  }
}

}  // namespace sss::dscore
