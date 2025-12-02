#include "WorkbenchLayout.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QStyle>
#include <QTabWidget>
#include <QToolButton>

#include "ModeSwitcher.h"
#include "OverlayCanvas.h"

namespace sss::dscore {

WorkbenchLayout::WorkbenchLayout(QWidget* parent) : QWidget(parent) {
  setupUi();

  // Create and configure the sidebar toggle button
  sidebar_toggle_btn_ = new QToolButton(this);  // Parent to WorkbenchLayout initially
  sidebar_toggle_btn_->setCheckable(false);
  sidebar_toggle_btn_->setAutoRaise(true);
  sidebar_toggle_btn_->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));  // Initial icon: pointing left (collapse)
  sidebar_toggle_btn_->setFixedSize(20, 40);                                  // Small button size
  sidebar_toggle_btn_->setStyleSheet(
      "QToolButton { background-color: rgba(0,0,0,50); border-radius: 5px; } QToolButton:hover { background-color: "
      "rgba(0,0,0,100); }");
  sidebar_toggle_btn_->setObjectName("sidebar_toggle_btn");  // For QSS styling

  connect(sidebar_toggle_btn_, &QToolButton::clicked, this, &WorkbenchLayout::onToggleSidebar);

  // Pass the button to OverlayCanvas to manage its position
  if (overlay_canvas_ != nullptr) {
    overlay_canvas_->SetSidebarToggleButton(sidebar_toggle_btn_);
  }
}

WorkbenchLayout::~WorkbenchLayout() = default;

void WorkbenchLayout::setupUi() {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  main_splitter_ = new QSplitter(Qt::Horizontal, this);

  // 1. Left Sidebar
  left_tab_widget_ = new QTabWidget(main_splitter_);
  left_tab_widget_->setObjectName("workbench_sidebar");
  left_tab_widget_->setTabPosition(QTabWidget::South);
  left_tab_widget_->setElideMode(Qt::ElideRight);

  // 2. Right Canvas (OverlayCanvas)
  overlay_canvas_ = new OverlayCanvas(main_splitter_);

  main_splitter_->addWidget(left_tab_widget_);
  main_splitter_->addWidget(overlay_canvas_);

  // Default Ratio and initial state
  main_splitter_->setCollapsible(0, true);
  main_splitter_->setStretchFactor(0, 0);  // Initially give left_tab_widget_ 0 stretch
  main_splitter_->setStretchFactor(1, 1);  // Give overlay_canvas_ 1 stretch (fills space)

  // Set initial sizes to ensure left_tab_widget_ has a visible size at start
  QList<int> initial_sizes;
  initial_sizes << last_sidebar_width_ << width() - last_sidebar_width_;
  main_splitter_->setSizes(initial_sizes);

  layout->addWidget(main_splitter_);

  // 3. Right Mode Switcher
  mode_switcher_ = new ModeSwitcher(this);
  layout->addWidget(mode_switcher_);

  connect(mode_switcher_, &ModeSwitcher::ModeSelected, this, [this](const QString& id) {
    if (mode_switch_callback_) {
      mode_switch_callback_(id);
    }
  });
}

void WorkbenchLayout::AddSidePanel(const QString& /*id*/, QWidget* panel, const QString& title, const QIcon& icon) {
  if (left_tab_widget_ != nullptr) {
    left_tab_widget_->addTab(panel, icon, title);
  }
}

void WorkbenchLayout::SetBackgroundWidget(QWidget* widget) {
  if (overlay_canvas_ != nullptr) overlay_canvas_->SetBackgroundWidget(widget);
}

void WorkbenchLayout::AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority,
                                       const QList<int>& visible_contexts, const QList<int>& enable_contexts) {
  if (overlay_canvas_ != nullptr)
    overlay_canvas_->AddSqueezeWidget(side, widget, priority, visible_contexts, enable_contexts);
}

void WorkbenchLayout::AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority,
                                       const QList<int>& visible_contexts, const QList<int>& enable_contexts) {
  if (overlay_canvas_ != nullptr)
    overlay_canvas_->AddOverlayWidget(zone, widget, priority, visible_contexts, enable_contexts);
}

void WorkbenchLayout::ShowNotification(const QString& message, int duration_ms) {
  if (overlay_canvas_ != nullptr) overlay_canvas_->ShowNotification(message, duration_ms);
}

void WorkbenchLayout::Clear() {
  // Clear Left Sidebar
  if (left_tab_widget_ != nullptr) {
    left_tab_widget_->clear();
  }

  // Clear Overlay Canvas
  if (overlay_canvas_ != nullptr) {
    overlay_canvas_->Clear();
  }
}

void WorkbenchLayout::AddModeButton(const QString& id, const QString& title, const QIcon& icon) {
  if (mode_switcher_ != nullptr) {
    mode_switcher_->AddModeButton(id, title, icon);
  }
}

void WorkbenchLayout::SetActiveModeButton(const QString& id) {
  if (mode_switcher_ != nullptr) {
    mode_switcher_->SetActiveMode(id);
  }
}

void WorkbenchLayout::SetModeSwitchCallback(std::function<void(const QString&)> callback) {
  mode_switch_callback_ = callback;
}

QSplitter* WorkbenchLayout::MainSplitter() const { return main_splitter_; }

void WorkbenchLayout::onToggleSidebar() {
  QList<int> sizes = main_splitter_->sizes();
  if (sizes.size() != 2) return;  // Expecting exactly two widgets in splitter

  if (sizes[0] > 0) {                                                            // Sidebar is currently open
    last_sidebar_width_ = sizes[0];                                              // Save current width
    sizes[0] = 0;                                                                // Collapse sidebar
    sidebar_toggle_btn_->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));  // Change icon to expand
  } else {                                                                       // Sidebar is currently closed
    sizes[0] = last_sidebar_width_;                                              // Restore width
    sidebar_toggle_btn_->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));   // Change icon to collapse
  }
  main_splitter_->setSizes(sizes);
}

}  // namespace sss::dscore
