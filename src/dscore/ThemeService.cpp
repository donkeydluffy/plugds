#include "ThemeService.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QFontInfo>
#include <QMetaEnum>
#include <QProxyStyle>  // Include for QProxyStyle comments
#include <QSettings>
#include <QStyleFactory>  // Include for QStyleFactory
#include <QWidget>
#include <algorithm>  // For std::sort
#include <utility>    // For std::pair

namespace sss::dscore {

// Helper to map string to QPalette::ColorRole
auto ThemeService::stringToPaletteColorRole(const QString& str) -> QPalette::ColorRole {
  static const QMetaObject& mo = QPalette::staticMetaObject;
  static int index = mo.indexOfEnumerator("ColorRole");
  static QMetaEnum meta_enum = mo.enumerator(index);

  // Use `keyToValue` for direct conversion from string to enum value
  int val = meta_enum.keyToValue(str.toLatin1().constData());
  if (val == -1) {
    qWarning() << "ThemeService: Unknown QPalette::ColorRole string:" << str;
    return QPalette::NColorRoles;  // Return an invalid role
  }
  return static_cast<QPalette::ColorRole>(val);
}

// Helper to map string to Theme::ColorRole
auto ThemeService::stringToThemeColorRole(const QString& str) -> Theme::ColorRole {
  // This map stores QString (like "THEME_COLOR_BrandColor") to Theme::ColorRole
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
      {"THEME_COLOR_TextDisabled", Theme::kTextDisabled}};
  Theme::ColorRole role = kStringToRoleMap.value(str, Theme::kCount);  // Correct: Use .value() with QString key
  if (role == Theme::kCount) {
    qWarning() << "ThemeService: Unknown Theme::ColorRole string for lookup:" << str;
  }
  return role;
}

// Helper to map Theme::ColorRole to string for QSS replacement
auto ThemeService::themeColorRoleToString(Theme::ColorRole role) -> QString {
  // This map stores Theme::ColorRole to QString (like "THEME_COLOR_BrandColor")
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
      {Theme::kTextDisabled, "THEME_COLOR_TextDisabled"}};
  return kRoleToStringMap.value(role, "UNKNOWN_COLOR_ROLE");
}

// Helper to map QPalette::ColorRole to string for QSS replacement
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
      {QPalette::AlternateBase, "PALETTE_AlternateBase"}  // Added for tables
  };
  return kMap.value(role, "UNKNOWN_PALETTE_ROLE");
}

ThemeService::ThemeService() {
  // Force Fusion style for consistent cross-platform theming support
  // Fusion respects QPalette much better than native styles (Windows/GTK)
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  // Set a consistent application font with a universal fallback
  QFont font("DejaVu Sans", 9);  // Use DejaVu Sans, common on Linux
  if (!font.exactMatch()) {
    font = QFont("sans-serif", 9);  // Generic sans-serif fallback
  }
  QApplication::setFont(font);

  // Initialize with a default theme instance, will be replaced on first load
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
  qDebug() << "ThemeService: Found" << palette_keys.size() << "palette entries.";

  if (palette_keys.isEmpty()) {
    qWarning() << "ThemeService: [Palette] section is empty or missing!";
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
}

auto ThemeService::Theme() const -> const sss::dscore::Theme* { return current_theme_.get(); }

auto ThemeService::GetColor(Theme::ColorRole role) const -> QColor {
  // Ensure current_theme_ is valid before dereferencing
  if (current_theme_) {
    return current_theme_->Color(role);
  }
  return {Qt::magenta};  // Fallback
}

auto ThemeService::applyPaletteToQapp() -> void {
  if (current_theme_) {
    QPalette new_palette = current_theme_->Palette();
    qApp->setPalette(new_palette);

    // 强制所有顶级窗口使用新调色板
    for (QWidget* widget : QApplication::topLevelWidgets()) {
      widget->setPalette(new_palette);
      widget->update();
    }
  }
}

auto ThemeService::applyStyleSheetToQapp() -> void {
  if (current_theme_) {
    const QString& stylesheet = current_theme_->StyleSheet();

    // 清除现有的样式表并应用新的
    qApp->setStyleSheet("");
    qApp->setStyleSheet(stylesheet);

    // 强制刷新所有顶级窗口和其子控件
    for (QWidget* widget : QApplication::topLevelWidgets()) {
      if (widget == nullptr) continue;

      // 强制样式重新应用
      widget->style()->unpolish(widget);
      widget->style()->polish(widget);

      // 刷新窗口
      widget->update();
      widget->repaint();

      // 递归刷新所有子控件
      QObjectList children = widget->children();
      for (QObject* child : children) {
        if (auto* child_widget = qobject_cast<QWidget*>(child)) {
          child_widget->style()->unpolish(child_widget);
          child_widget->style()->polish(child_widget);
          child_widget->update();
        }
      }
    }

    // 发送样式更改事件给所有窗口
    QEvent style_event(QEvent::StyleChange);
    for (QWidget* widget : QApplication::topLevelWidgets()) {
      if (widget != nullptr) {
        QApplication::sendEvent(widget, &style_event);
      }
    }
  }
}

}  // namespace sss::dscore
