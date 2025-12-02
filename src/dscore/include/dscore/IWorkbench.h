#pragma once

#include <QIcon>
#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
class QString;
QT_END_NAMESPACE

namespace sss::dscore {

// Defines where an overlay widget anchors.
enum class OverlayZone : uint8_t {
  kTopLeft,
  kTopRight,
  kBottomLeft,
  kBottomCenter,
  kTopCenter,
  kCenter,
  kTop,
  kBottom,
  kLeft,
  kRight
};

// Defines where a squeeze widget docks.
enum class SqueezeSide : uint8_t { kTop, kBottom, kLeft, kRight };

/**
 * @brief The IWorkbench interface defines the contract for a standard workspace layout.
 * Plugins can use this interface to inject UI elements into the active workspace.
 */
class IWorkbench {
 public:
  virtual ~IWorkbench() = default;

  /**
   * @brief Adds a new tab to the left sidebar.
   * @param id Unique identifier for this panel.
   * @param panel The widget to display.
   * @param title The title of the tab.
   * @param icon The icon of the tab (optional).
   */
  virtual void AddSidePanel(const QString& id, QWidget* panel, const QString& title, const QIcon& icon = QIcon()) = 0;

  /**
   * @brief Sets the central background widget (e.g., 3D View).
   */
  virtual void SetBackgroundWidget(QWidget* widget) = 0;

  /**
   * @brief Adds a widget that pushes the background area (Squeeze).
   * @param side The side to dock the widget.
   * @param widget The widget to add.
   * @param priority The sort priority (Higher values = closer to the edge/outermost).
   * @param visible_contexts List of context IDs where this widget is visible. Empty = Global.
   * @param enable_contexts List of context IDs where this widget is enabled. Empty = Global.
   */
  virtual void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority = 0,
                                const QList<int>& visible_contexts = {}, const QList<int>& enable_contexts = {}) = 0;

  /**
   * @brief Adds a floating overlay widget.
   * @param zone The zone to anchor the widget.
   * @param widget The widget to add.
   * @param priority The sort priority (Higher values = higher in stack or Z-order).
   * @param visible_contexts List of context IDs where this widget is visible. Empty = Global.
   * @param enable_contexts List of context IDs where this widget is enabled. Empty = Global.
   */
  virtual void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority = 0,
                                const QList<int>& visible_contexts = {}, const QList<int>& enable_contexts = {}) = 0;

  /**
   * @brief Shows a transient notification message in the top-center area.
   * @param message The text to display.
   * @param duration_ms Duration in milliseconds.
   */
  virtual void ShowNotification(const QString& message, int duration_ms = 3000) = 0;

  /**
   * @brief Clears all content (Side panels, Squeeze, Overlays) from the workbench.
   */
  virtual void Clear() = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IWorkbench, "sss.dscore.IWorkbench")
