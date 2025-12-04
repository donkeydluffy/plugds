#pragma once

#include <QColor>
#include <QObject>

#include "dscore/CoreSpec.h"
#include "dscore/Theme.h"

namespace sss::dscore {

/**
 * @brief 主题服务接口
 * 负责管理全局UI主题，包括 QPalette、QSS 样式表和语义颜色。
 */
class DS_CORE_DLLSPEC IThemeService : public QObject {
  Q_OBJECT

 public:
  ~IThemeService() override = default;

  /**
   * @brief 通过ID加载并应用主题。
   * 读取相应的主题配置（INI）和QSS模板，
   * 然后将生成的QPalette和QSS应用到qApp。
   *
   * @param theme_id 主题标识符（例如："dark", "light"）。
   */
  virtual auto LoadTheme(const QString& theme_id) -> void = 0;

  /**
   * @brief 获取当前活动的主题对象。
   * 提供对完整主题数据的访问，用于自定义绘制或高级查询。
   *
   * @returns const Theme* 指向当前主题对象的指针。
   */
  [[nodiscard]] virtual auto Theme() const -> const Theme* = 0;

  /**
   * @brief 获取由当前主题定义的语义颜色。
   * 允许组件使用强类型角色语义化地获取颜色。
   *
   * @param role 颜色角色（例如：Theme::BrandColor, Theme::PanelBackground）。
   * @returns QColor 相应的颜色对象。
   */
  [[nodiscard]] virtual auto GetColor(Theme::ColorRole role) const -> QColor = 0;

  /**
   * @brief 获取主题化图标。
   *
   * @param base_path 资源基础路径（例如：":/dscore/resources/icons"）。
   * @param icon_name 图标文件名（例如："file_open.svg"）。
   * @return QIcon 来自正确主题子目录（浅色/深色）的图标。
   */
  [[nodiscard]] virtual auto GetIcon(const QString& base_path, const QString& icon_name) const -> QIcon = 0;

 signals:
  /**
   * @brief 当主题更改时发出。
   * @param theme_id 新主题的ID。
   */
  void ThemeChanged(const QString& theme_id);
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IThemeService, "sss.dscore.IThemeService/2.0.0")
