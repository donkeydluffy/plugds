#include "dscore/Theme.h"

#include <utility>

namespace sss::dscore {

Theme::Theme(QString id) : id_(std::move(id)), colors_(ColorRole::kCount) {
  // 初始化默认 Palette
  // 我们不完全依赖系统默认值，而是给一个合理的初始状态
  palette_ = QPalette();

  // 初始化语义颜色
  // 使用洋红色 (Magenta) 作为默认值，这样如果某个颜色忘了在 INI 里配置，
  // 在界面上会非常显眼 (Fail-Fast 视觉原则)
  colors_.fill(QColor(Qt::magenta));
}

auto Theme::Color(ColorRole role) const -> QColor {
  if (role >= 0 && role < colors_.size()) {
    return colors_[role];
  }
  // 越界保护
  return {Qt::magenta};
}

auto Theme::SetPaletteColor(QPalette::ColorGroup group, QPalette::ColorRole role, const QColor& color) -> void {
  palette_.setColor(group, role, color);
}

auto Theme::SetColor(ColorRole role, const QColor& color) -> void {
  if (role >= 0 && role < colors_.size()) {
    colors_[role] = color;
  }
}

auto Theme::SetStyleSheet(const QString& qss) -> void { generated_style_sheet_ = qss; }

}  // namespace sss::dscore
