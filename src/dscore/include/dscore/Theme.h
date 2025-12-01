#pragma once

#include <QColor>
#include <QPalette>
#include <QString>
#include <QVector>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class DS_CORE_DLLSPEC Theme {
 public:
  // 语义颜色角色
  // 这些角色不仅用于 C++ 绘图，也会作为变量名 (e.g., @BrandColor) 出现在 QSS 模板中
  enum ColorRole : uint8_t {
    // --- 基础品牌色 ---
    kBrandColor,         // 主色调 (e.g., 蓝色)
    kBrandColorHover,    // 主色调悬停
    kBrandColorPressed,  // 主色调按下

    // --- 功能状态色 ---
    kErrorColor,    // 错误 (e.g., 红色)
    kWarningColor,  // 警告 (e.g., 橙色/黄色)
    kSuccessColor,  // 成功 (e.g., 绿色)
    kInfoColor,     // 信息 (e.g., 浅蓝)

    // --- 界面构造 (Panel & Containers) ---
    kPanelBackground,  // 一般面板/工具栏背景
    kPanelBorder,      // 面板分割线/边框

    // --- 内容编辑器 (Editor/Canvas) ---
    kEditorBackground,  // 编辑区背景 (通常比面板更深或更亮)
    kEditorTextColor,   // 编辑区文字
    kEditorLineNumber,  // 行号颜色
    kEditorSelection,   // 选区背景色

    // --- 滚动条 (QScrollBar) ---
    kScrollBarBackground,
    kScrollBarHandle,
    kScrollBarHandleHover,

    // --- 菜单与列表 (QMenu, QListView) ---
    kMenuBackground,
    kMenuBorder,
    kMenuItemHover,
    kMenuItemText,

    // --- 文本辅助 ---
    kTextPrimary,    // 主要文字 (通常对应 QPalette::WindowText)
    kTextSecondary,  // 次要文字 (注释、提示)
    kTextDisabled,   // 禁用文字

    // 必须是最后一个
    kCount
  };

  explicit Theme(QString id);
  ~Theme() = default;

  [[nodiscard]] auto Id() const -> QString { return id_; }

  // 获取用于 qApp->setPalette 的调色板 (标准控件颜色)
  [[nodiscard]] auto Palette() const -> QPalette { return palette_; }

  // 获取语义颜色 (自定义绘制颜色)
  [[nodiscard]] auto Color(ColorRole role) const -> QColor;

  // 获取生成的最终 QSS (包含替换后的颜色值)
  [[nodiscard]] auto StyleSheet() const -> QString { return generated_style_sheet_; }

  // --- Setters (供 ThemeService 加载逻辑使用) ---
  auto SetPaletteColor(QPalette::ColorGroup group, QPalette::ColorRole role, const QColor& color) -> void;
  auto SetColor(ColorRole role, const QColor& color) -> void;
  auto SetStyleSheet(const QString& qss) -> void;

 private:
  QString id_;
  QPalette palette_;
  QVector<QColor> colors_;
  QString generated_style_sheet_;  // 修正变量命名风格
};

}  // namespace sss::dscore
