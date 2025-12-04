#pragma once

#include <QMap>
#include <QToolButton>
#include <QVector>
#include <QWidget>

#include "dscore/IWorkbench.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
QT_END_NAMESPACE

namespace sss::dscore {

/**
 * @brief 一个自定义布局容器，管理：
 * 1. 背景层：填充剩余空间。
 * 2. 挤压层：停靠在侧边的小部件，减少背景的区域。
 * 3. 覆盖层：浮动在所有内容上方的小部件，锚定到角落/中心。
 */
class OverlayCanvas : public QWidget {
  Q_OBJECT

 public:
  explicit OverlayCanvas(QWidget* parent = nullptr);
  ~OverlayCanvas() override;

  /**
   * @brief 设置中央背景小部件（例如，3D 视图）。
   * 该小部件将填充挤压小部件放置后剩余的空间。
   */
  void SetBackgroundWidget(QWidget* widget);

  /**
   * @brief 添加一个"挤压"背景区域的小部件。
   */
  void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {});

  /**
   * @brief 添加一个浮动在所有内容上方的小部件。
   */
  void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {});

  /**
   * @brief 显示一个临时通知消息。
   */
  void ShowNotification(const QString& message, int duration_ms = 3000);

  /**
   * @brief 清除所有已注册的小部件（挤压和覆盖）并重置状态。
   * 不删除小部件，只是将它们从布局管理中移除。
   */
  void Clear();

  /**
   * @brief 设置切换侧边栏可见性的按钮。
   * 该按钮将定位在画布的中心左边缘。
   * @param button 要管理的 QToolButton。
   */
  void SetSidebarToggleButton(QToolButton* button);

 public slots:  // NOLINT
  /**
   * @brief 根据当前上下文更新所有小部件的可见性/启用状态。
   */
  void UpdateContextState();

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  struct SqueezeItem {
    QWidget* widget;
    SqueezeSide side;
    int priority;
    QList<int> visible_contexts;
    QList<int> enable_contexts;
  };

  struct OverlayItem {
    QWidget* widget;
    OverlayZone zone;
    int priority;
    QList<int> visible_contexts;
    QList<int> enable_contexts;
  };

  // 背景
  QWidget* background_widget_ = nullptr;

  // 挤压小部件
  QVector<SqueezeItem> squeeze_widgets_;

  // 覆盖项（现在手动管理以进行排序）
  QVector<OverlayItem> overlay_items_;

  // 覆盖容器
  // 我们使用带有布局的内部小部件来自动管理每个区域中的堆叠。
  QMap<OverlayZone, QWidget*> overlay_containers_;

  void initOverlayContainers();
  void layoutSqueezeWidgets(const QRect& area, QRect& remaining_rect);
  void layoutOverlayWidgets(const QRect& area);

  // 获取或创建容器的辅助函数
  QWidget* getOverlayContainer(OverlayZone zone);

  // 基于排序项重建特定覆盖容器的布局
  void refreshOverlayContainer(OverlayZone zone);

  QWidget* notification_widget_ = nullptr;

  // 侧边栏切换按钮（由 OverlayCanvas 管理和定位）
  QToolButton* sidebar_toggle_button_ = nullptr;
};

}  // namespace sss::dscore
