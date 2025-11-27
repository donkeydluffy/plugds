#include "ThemeService.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

namespace sss::dsresources {

ThemeService::ThemeService() = default;

auto ThemeService::LoadTheme(const QString& theme_id) -> void {
  // Simple implementation: load qss from resource
  QString path = QString(":/dsresources/resources/themes/%1.qss").arg(theme_id);
  QFile file(path);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QString style = QLatin1String(file.readAll());
    qApp->setStyleSheet(style);
    qDebug() << "Loaded theme from:" << path;
  } else {
    qWarning() << "Failed to load theme:" << path;
  }

  // Initialize palette/fonts for the theme (Hardcoded for prototype)
  // In a real system, these would be parsed from the QSS or a separate theme config file
  if (theme_id == "dark") {
    current_colors_["primary"] = QColor("#007acc");
    current_colors_["error"] = QColor("#ff3333");
  } else {
    current_colors_["primary"] = QColor("#005a9e");
    current_colors_["error"] = QColor("#cc0000");
  }
}

auto ThemeService::GetColor(const QString& role) const -> QColor {
  return current_colors_.value(role, QColor(Qt::black));
}

auto ThemeService::GetFont(const QString& role) const -> QFont {
  // Default font
  return QApplication::font();
}

}  // namespace sss::dsresources
