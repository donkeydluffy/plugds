#pragma once

#include <QMap>
#include <QToolButton>  // Required for QToolButton
#include <QVector>
#include <QWidget>

#include "dscore/IWorkbench.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
QT_END_NAMESPACE

namespace sss::dscore {

/**
 * @brief A custom layout container that manages:
 * 1. Background Layer: Fills the remaining space.
 * 2. Squeeze Layer: Widgets docked to sides that reduce the Background's area.
 * 3. Overlay Layer: Widgets floating on top of everything, anchored to corners/center.
 */
class OverlayCanvas : public QWidget {
  Q_OBJECT

 public:
  explicit OverlayCanvas(QWidget* parent = nullptr);
  ~OverlayCanvas() override;

  /**
   * @brief Sets the central background widget (e.g., 3D View).
   * This widget will fill the space remaining after squeeze widgets are placed.
   */
  void SetBackgroundWidget(QWidget* widget);

  /**
   * @brief Adds a widget that "squeezes" the background area.
   */
  void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {});

  /**
   * @brief Adds a widget that floats over everything.
   */
  void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {});

  /**
   * @brief Shows a transient notification message.
   */
  void ShowNotification(const QString& message, int duration_ms = 3000);

  /**
   * @brief Clears all registered widgets (Squeeze and Overlay) and resets the state.
   * Does not delete the widgets, just removes them from layout management.
   */
  void Clear();

  /**
   * @brief Sets the button that toggles the sidebar visibility.
   * This button will be positioned at the center-left edge of the canvas.
   * @param button The QToolButton to manage.
   */
  void SetSidebarToggleButton(QToolButton* button);

 public slots:  // NOLINT
  /**
   * @brief Updates the visibility/enabled state of all widgets based on the current context.
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

  // Background
  QWidget* background_widget_ = nullptr;

  // Squeeze Widgets
  QVector<SqueezeItem> squeeze_widgets_;

  // Overlay Items (Managed manually now for sorting)
  QVector<OverlayItem> overlay_items_;

  // Overlay Containers
  // We use internal widgets with Layouts to manage stacking in each zone automatically.
  QMap<OverlayZone, QWidget*> overlay_containers_;

  void initOverlayContainers();
  void layoutSqueezeWidgets(const QRect& area, QRect& remaining_rect);
  void layoutOverlayWidgets(const QRect& area);

  // Helper to get or create container
  QWidget* getOverlayContainer(OverlayZone zone);

  // Rebuilds the layout of a specific overlay container based on sorted items
  void refreshOverlayContainer(OverlayZone zone);

  QWidget* notification_widget_ = nullptr;

  // Sidebar toggle button (managed and positioned by OverlayCanvas)
  QToolButton* sidebar_toggle_button_ = nullptr;
};

}  // namespace sss::dscore
