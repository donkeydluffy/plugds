#pragma once

#include <QIcon>
#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
class QString;
QT_END_NAMESPACE

namespace sss::dscore {

// 定义覆盖小部件锚定的位置。
enum class OverlayZone : uint8_t {
  kTopLeft,
  kTopRight,
  kBottomLeft,
  kBottomRight,
  kTopCenter,
  kBottomCenter,
  kLeftCenter,
  kRightCenter,
  kCenter,
  kTop,
  kBottom,
  kLeft,
  kRight
};

// 定义挤压小部件停靠的位置。
enum class SqueezeSide : uint8_t { kTop, kBottom, kLeft, kRight };

/**
 * @brief       IWorkbench 接口定义了标准工作区布局的契约。
 * @details     插件可以使用此接口将UI元素注入到活动工作区中。
 * @class       sss::dscore::IWorkbench IWorkbench.h <IWorkbench>
 */
class IWorkbench {
 public:
  virtual ~IWorkbench() = default;

  /**
   * @brief       向左侧边栏添加新选项卡。
   * @param[in]   id 此面板的唯一标识符。
   * @param[in]   panel 要显示的小部件。
   * @param[in]   title 选项卡的标题。
   * @param[in]   icon 选项卡的图标（可选）。
   */
  virtual void AddSidePanel(const QString& id, QWidget* panel, const QString& title, const QIcon& icon = QIcon()) = 0;

  /**
   * @brief       设置中央背景小部件（例如 3D 视图）。
   * @param[in]   widget 要设置的背景小部件。
   */
  virtual void SetBackgroundWidget(QWidget* widget) = 0;

  /**
   * @brief       添加挤压背景区域的小部件。
   * @details     此类型的小部件会从指定方向挤压背景区域。
   * @param[in]   side 要停靠小部件的边。
   * @param[in]   widget 要添加的小部件。
   * @param[in]   priority 排序优先级（数值越高越靠近边缘/最外层）。
   * @param[in]   visible_contexts 此小部件可见的上下文ID列表。空列表表示全局可见。
   * @param[in]   enable_contexts 此小部件启用的上下文ID列表。空列表表示全局启用。
   */
  virtual void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority = 0,
                                const QList<int>& visible_contexts = {}, const QList<int>& enable_contexts = {}) = 0;

  /**
   * @brief       添加浮动覆盖小部件。
   * @param[in]   zone 要锚定小部件的区域。
   * @param[in]   widget 要添加的小部件。
   * @param[in]   priority 排序优先级（数值越高在堆栈或Z顺序中越高）。
   * @param[in]   visible_contexts 此小部件可见的上下文ID列表。空列表表示全局可见。
   * @param[in]   enable_contexts 此小部件启用的上下文ID列表。空列表表示全局启用。
   */
  virtual void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority = 0,
                                const QList<int>& visible_contexts = {}, const QList<int>& enable_contexts = {}) = 0;

  /**
   * @brief       在顶部中心区域显示瞬时通知消息。
   * @param[in]   message 要显示的文本。
   * @param[in]   duration_ms 持续时间（毫秒）。
   */
  virtual void ShowNotification(const QString& message, int duration_ms = 3000) = 0;

  /**
   * @brief       从工作台中清除所有内容（侧面板、挤压小部件、覆盖小部件）。
   */
  virtual void Clear() = 0;

  /**
   * @brief       向右侧边栏添加模式切换按钮。
   * @param[in]   id 模式的唯一标识符。
   * @param[in]   title 工具提示标题。
   * @param[in]   icon 按钮图标。
   */
  virtual void AddModeButton(const QString& id, const QString& title, const QIcon& icon) = 0;

  /**
   * @brief       设置模式按钮的活动状态。
   * @param[in]   id 模式的唯一标识符。
   */
  virtual void SetActiveModeButton(const QString& id) = 0;

  /**
   * @brief       设置模式按钮被点击时的回调函数。
   */
  virtual void SetModeSwitchCallback(std::function<void(const QString&)> callback) = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IWorkbench, "sss.dscore.IWorkbench")
