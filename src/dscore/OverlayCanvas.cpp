#include "OverlayCanvas.h"

#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

#include "dscore/IContextManager.h"

namespace sss::dscore {

OverlayCanvas::OverlayCanvas(QWidget* parent) : QWidget(parent) {
  // OverlayCanvas manages its own layout manually in resizeEvent
  initOverlayContainers();

  auto* cm = sss::dscore::IContextManager::GetInstance();
  if (cm != nullptr) {
    connect(cm, &IContextManager::ContextChanged, this, &OverlayCanvas::UpdateContextState);
  }
}

OverlayCanvas::~OverlayCanvas() = default;

void OverlayCanvas::ShowNotification(const QString& message, int duration_ms) {
  if (notification_widget_ != nullptr) {
    notification_widget_->hide();
    notification_widget_->deleteLater();
  }

  notification_widget_ = new QLabel(message, this);
  notification_widget_->setStyleSheet(
      "QLabel { background-color: #333333; color: white; padding: 8px 16px; border-radius: 4px; border: 1px solid "
      "#555555; }");
  notification_widget_->adjustSize();

  // Initial position (will be fixed by layout, but set here for immediate show)
  int x = (width() - notification_widget_->width()) / 2;
  int y = 20;  // Slight margin from top
  notification_widget_->move(x, y);

  auto* effect = new QGraphicsOpacityEffect(notification_widget_);
  notification_widget_->setGraphicsEffect(effect);

  notification_widget_->show();
  notification_widget_->raise();

  auto* anim_in = new QPropertyAnimation(effect, "opacity", notification_widget_);
  anim_in->setDuration(300);
  anim_in->setStartValue(0.0);
  anim_in->setEndValue(1.0);
  anim_in->start(QAbstractAnimation::DeleteWhenStopped);

  QTimer::singleShot(duration_ms, notification_widget_, [this, effect]() {
    if (!notification_widget_) return;
    auto* anim_out = new QPropertyAnimation(effect, "opacity", notification_widget_);
    anim_out->setDuration(300);
    anim_out->setStartValue(1.0);
    anim_out->setEndValue(0.0);
    connect(anim_out, &QPropertyAnimation::finished, notification_widget_, &QWidget::deleteLater);
    connect(anim_out, &QPropertyAnimation::finished, [this]() { notification_widget_ = nullptr; });
    anim_out->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

void OverlayCanvas::Clear() {
  // Clear Squeeze Widgets
  for (auto& item : squeeze_widgets_) {
    if (item.widget != nullptr) {
      item.widget->hide();
      item.widget->setParent(nullptr);  // Detach from canvas
    }
  }
  squeeze_widgets_.clear();

  // Clear Overlay Widgets
  for (auto& item : overlay_items_) {
    if (item.widget != nullptr) {
      item.widget->hide();
      item.widget->setParent(nullptr);  // Detach from canvas
    }
  }
  overlay_items_.clear();

  // Clear containers
  for (auto* container : overlay_containers_) {
    if (container != nullptr && container->layout() != nullptr) {
      QLayoutItem* child = nullptr;
      while ((child = container->layout()->takeAt(0)) != nullptr) {
        delete child;
      }
    }
  }

  // Reset Background (optional, usually controlled by SetBackgroundWidget)
  // SetBackgroundWidget(nullptr);

  update();
}

void OverlayCanvas::SetSidebarToggleButton(QToolButton* button) {
  if (sidebar_toggle_button_ == button) {
    return;
  }

  if (sidebar_toggle_button_ != nullptr) {
    sidebar_toggle_button_->hide();
    sidebar_toggle_button_->setParent(nullptr);
  }

  sidebar_toggle_button_ = button;
  if (sidebar_toggle_button_ != nullptr) {
    sidebar_toggle_button_->setParent(this);
    sidebar_toggle_button_->show();
    sidebar_toggle_button_->raise();  // Ensure it's always on top
  }
  update();  // Trigger relayout
}

void OverlayCanvas::SetBackgroundWidget(QWidget* widget) {
  if (background_widget_ == widget) {
    return;
  }

  // Remove old
  if (background_widget_ != nullptr) {
    background_widget_->hide();
    background_widget_->setParent(nullptr);
  }

  background_widget_ = widget;
  if (background_widget_ != nullptr) {
    background_widget_->setParent(this);
    background_widget_->lower();  // Ensure it is at the bottom
    background_widget_->show();
  }
  update();  // Trigger relayout
}

void OverlayCanvas::AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority,
                                     const QList<int>& visible_contexts, const QList<int>& enable_contexts) {
  if (widget == nullptr) {
    return;
  }

  widget->setParent(this);
  // Visibility will be set by updateContextState

  squeeze_widgets_.append({widget, side, priority, visible_contexts, enable_contexts});

  // Sort squeeze widgets by priority (descending)
  // Higher priority = Processed first = Outermost position
  std::stable_sort(squeeze_widgets_.begin(), squeeze_widgets_.end(),
                   [](const SqueezeItem& a, const SqueezeItem& b) { return a.priority > b.priority; });

  UpdateContextState();  // Initial state check
}

void OverlayCanvas::AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority,
                                     const QList<int>& visible_contexts, const QList<int>& enable_contexts) {
  if (widget == nullptr) {
    return;
  }

  overlay_items_.append({widget, zone, priority, visible_contexts, enable_contexts});

  refreshOverlayContainer(zone);
  UpdateContextState();
}

void OverlayCanvas::UpdateContextState() {
  QList<int> active_contexts;
  auto* cm = sss::dscore::IContextManager::GetInstance();
  if (cm != nullptr) {
    active_contexts = cm->GetActiveContexts();
  }
  // Ensure Global context (0) is always considered active for matching
  if (!active_contexts.contains(0)) {
    active_contexts.append(0);
  }

  // Helper: Check if active_contexts matches any ID in required_contexts (Intersection).
  // Empty required_contexts implies Global/Always active.
  auto is_active = [&](const QList<int>& required_contexts) -> bool {
    if (required_contexts.isEmpty()) return true;
    // If it contains 0, it's always active
    if (required_contexts.contains(0)) return true;

    return std::any_of(std::begin(required_contexts), std::end(required_contexts),
                       [&](int id) { return active_contexts.contains(id); });
  };

  // Update Squeeze Widgets
  for (const auto& item : squeeze_widgets_) {
    bool visible = is_active(item.visible_contexts);
    bool enabled = is_active(item.enable_contexts);

    item.widget->setVisible(visible);
    // Only change enabled state if visible (optimization)
    if (visible) {
      item.widget->setEnabled(enabled);
    }
  }

  // Update Overlay Widgets
  for (const auto& item : overlay_items_) {
    bool visible = is_active(item.visible_contexts);
    bool enabled = is_active(item.enable_contexts);

    item.widget->setVisible(visible);
    if (visible) {
      item.widget->setEnabled(enabled);
    }
  }

  // Trigger relayout
  update();
  // Force layout recalculation as visibility might have changed
  QResizeEvent event(size(), size());
  resizeEvent(&event);
}

void OverlayCanvas::initOverlayContainers() {
  auto create_container = [&](OverlayZone zone) {
    auto* c = new QWidget(this);
    c->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    c->setAttribute(Qt::WA_TranslucentBackground);

    QBoxLayout* layout = nullptr;

    switch (zone) {
      case OverlayZone::kTopLeft:
      case OverlayZone::kTopRight:
      case OverlayZone::kBottomLeft:
      case OverlayZone::kBottomCenter:
      case OverlayZone::kTopCenter:
      case OverlayZone::kTop:
      case OverlayZone::kBottom:
        if (zone == OverlayZone::kBottomCenter || zone == OverlayZone::kTopCenter || zone == OverlayZone::kTop ||
            zone == OverlayZone::kBottom) {
          layout = new QHBoxLayout(c);
        } else {
          layout = new QVBoxLayout(c);
        }
        break;
      case OverlayZone::kLeft:
      case OverlayZone::kRight:
        layout = new QVBoxLayout(c);  // Left/Right zones should stack vertically
        break;
      case OverlayZone::kCenter:
        layout = new QVBoxLayout(c);
        break;
    }

    if (layout != nullptr) {
      layout->setContentsMargins(5, 5, 5, 5);
      layout->setSpacing(5);
    }

    c->setLayout(layout);
    c->hide();
    overlay_containers_[zone] = c;
  };

  create_container(OverlayZone::kTopLeft);
  create_container(OverlayZone::kTopRight);
  create_container(OverlayZone::kBottomLeft);
  create_container(OverlayZone::kBottomCenter);
  create_container(OverlayZone::kTopCenter);
  create_container(OverlayZone::kTop);
  create_container(OverlayZone::kBottom);
  create_container(OverlayZone::kLeft);
  create_container(OverlayZone::kRight);
}

QWidget* OverlayCanvas::getOverlayContainer(OverlayZone zone) {
  if (!overlay_containers_.contains(zone)) {
    return nullptr;
  }
  QWidget* c = overlay_containers_[zone];
  c->show();
  c->raise();
  return c;
}

void OverlayCanvas::refreshOverlayContainer(OverlayZone zone) {
  QWidget* container = getOverlayContainer(zone);
  if (container == nullptr) {
    return;
  }

  auto* layout = qobject_cast<QBoxLayout*>(container->layout());
  if (layout == nullptr) {
    return;
  }

  // Clear layout (remove items without deleting widgets)
  QLayoutItem* child = nullptr;
  while ((child = layout->takeAt(0)) != nullptr) {
    delete child;  // The LayoutItem wrapper, not the widget
  }

  // Filter items for this zone
  QVector<OverlayItem*> zone_items;
  for (auto& item : overlay_items_) {
    if (item.zone == zone) {
      zone_items.append(&item);
    }
  }

  // Sort by priority (Higher priority first = Top of stack)
  std::stable_sort(zone_items.begin(), zone_items.end(),
                   [](const OverlayItem* a, const OverlayItem* b) { return a->priority > b->priority; });

  // Add to layout
  for (auto* item : zone_items) {
    layout->addWidget(item->widget);
    // Visibility/Enabled state will be set by updateContextState
  }
  container->adjustSize();
}

void OverlayCanvas::resizeEvent(QResizeEvent* /*event*/) {
  QRect available_rect = rect();

  layoutSqueezeWidgets(rect(), available_rect);

  if (background_widget_ != nullptr) {
    background_widget_->setGeometry(available_rect);
  }

  layoutOverlayWidgets(available_rect);
}

void OverlayCanvas::layoutSqueezeWidgets(const QRect& /*total_area*/, QRect& remaining_rect) {
  for (const auto& item : squeeze_widgets_) {
    QWidget* w = item.widget;
    if (!w->isVisible()) {
      continue;
    }

    QSize hint = w->sizeHint();
    QRect placement;
    switch (item.side) {
      case SqueezeSide::kTop:
        placement = QRect(remaining_rect.left(), remaining_rect.top(), remaining_rect.width(), hint.height());
        remaining_rect.setTop(remaining_rect.top() + hint.height());
        break;
      case SqueezeSide::kBottom:
        placement = QRect(remaining_rect.left(), remaining_rect.bottom() - hint.height() + 1, remaining_rect.width(),
                          hint.height());
        remaining_rect.setBottom(remaining_rect.bottom() - hint.height());
        break;
      case SqueezeSide::kLeft:
        placement = QRect(remaining_rect.left(), remaining_rect.top(), hint.width(), remaining_rect.height());
        remaining_rect.setLeft(remaining_rect.left() + hint.width());
        break;
      case SqueezeSide::kRight:
        placement = QRect(remaining_rect.right() - hint.width() + 1, remaining_rect.top(), hint.width(),
                          remaining_rect.height());
        remaining_rect.setRight(remaining_rect.right() - hint.width());
        break;
    }
    w->setGeometry(placement);
  }
}

void OverlayCanvas::layoutOverlayWidgets(const QRect& area) {
  for (auto it = overlay_containers_.begin(); it != overlay_containers_.end(); ++it) {
    QWidget* c = it.value();
    if (!c->isVisible() || c->layout()->count() == 0) {
      c->hide();
      continue;
    }
    c->show();
    c->adjustSize();

    int w = c->width();
    int h = c->height();
    int x = 0;
    int y = 0;

    switch (it.key()) {
      case OverlayZone::kTopLeft:
        x = area.left();
        y = area.top();
        break;
      case OverlayZone::kTopRight:
        x = area.right() - w;
        y = area.top();
        break;
      case OverlayZone::kBottomLeft:
        x = area.left();
        y = area.bottom() - h;
        break;
      case OverlayZone::kBottomCenter:
        x = area.center().x() - (w / 2);
        y = area.bottom() - h;
        break;
      case OverlayZone::kTopCenter:
        x = area.center().x() - (w / 2);
        y = area.top();
        break;
      case OverlayZone::kCenter:
        x = area.center().x() - (w / 2);
        y = area.center().y() - (h / 2);
        break;
      case OverlayZone::kTop:  // Full width at top
        x = area.left();
        y = area.top();
        w = area.width();  // Span full width
        break;
      case OverlayZone::kBottom:  // Full width at bottom
        x = area.left();
        y = area.bottom() - h;
        w = area.width();  // Span full width
        break;
      case OverlayZone::kLeft:  // Full height on left
        x = area.left();
        y = area.top();
        h = area.height();  // Span full height
        break;
      case OverlayZone::kRight:  // Full height on right
        x = area.right() - w;
        y = area.top();
        h = area.height();  // Span full height
        break;
    }

    c->move(x, y);
    c->raise();
  }

  if (notification_widget_ != nullptr) {
    notification_widget_->adjustSize();
    int x = (width() - notification_widget_->width()) / 2;
    int y = 20;
    notification_widget_->move(x, y);
    notification_widget_->raise();
  }

  if ((sidebar_toggle_button_ != nullptr) && sidebar_toggle_button_->isVisible()) {
    int button_width = sidebar_toggle_button_->width();
    int button_height = sidebar_toggle_button_->height();
    int margin = 5;

    sidebar_toggle_button_->setGeometry(margin, (height() - button_height) / 2, button_width, button_height);
    sidebar_toggle_button_->raise();
  }
}

}  // namespace sss::dscore
