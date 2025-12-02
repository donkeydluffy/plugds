#include "Ws1Page.h"

#include <spdlog/spdlog.h>

#include <QEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#include "dscore/CollapsibleWidget.h"
#include "dscore/IContextManager.h"
// #include "dscore/IPageManager.h" // Removed
#include "dscore/IThemeService.h"
#include "dscore/IWorkbench.h"  // Needed to access global workbench
#include "extsystem/IComponentManager.h"

namespace sss::ws1 {

Ws1Page::Ws1Page(int context_id, QObject* parent) : sss::dscore::IMode(parent), context_id_(context_id) {
  SPDLOG_INFO("Ws1Page (Mode) constructor called.");

  // Connect to ThemeService
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws1Page::UpdateIcons);
  }

  setupDefaultUi();  // Create widgets once
}

Ws1Page::~Ws1Page() {
  // Widgets are owned by us or Qt parent?
  // If we passed them to Workbench, they might be reparented.
  // We should be careful about ownership.
  // For simplicity in this prototype, we rely on QObject parent mechanism or memory leak risk if not careful.
  // Ideally, when Deactivate is called, we take them back or hide them.
  // But if Workbench::Clear() deletes them, we are in trouble if we hold pointers.
  // Let's assume Workbench::Clear() does NOT delete widgets, just removes them.
  // Our OverlayCanvas::Clear() removes them from layout and hides them. It does NOT delete them unless they are
  // children of a deleted layout item? "delete child" in OverlayCanvas::Clear refers to QLayoutItem, not the widget. So
  // we retain ownership.
  delete tree_view_;
  delete bg_label_;
  delete device_panel_;
  delete func_bar_;
  delete coords_label_;
  delete model_;
}

QString Ws1Page::Id() const { return "ws1.mode"; }
QString Ws1Page::Title() const { return tr("Workspace 1"); }
QIcon Ws1Page::Icon() const { return QIcon(":/ws1/resources/icons/sample.svg"); }  // Placeholder
int Ws1Page::ContextId() const { return context_id_; }
int Ws1Page::Priority() const { return 10; }

void Ws1Page::Activate() {
  SPDLOG_INFO("Ws1Page::Activate called.");
  auto* workbench = sss::extsystem::GetTObject<sss::dscore::IWorkbench>();
  if (workbench == nullptr) {
    SPDLOG_ERROR("Global IWorkbench not found!");
    return;
  }

  // 1. Left Sidebar
  workbench->AddSidePanel("ws1.sidebar.tree", tree_view_, tr("Model Tree"), QIcon{});

  // 2. Background
  workbench->SetBackgroundWidget(bg_label_);

  // 3. Overlays
  // Note: We can use context-aware adding here.
  // Since this Mode is active, its ContextId is active.
  // We can bind widgets to this mode's context or sub-contexts.

  // Global to this Mode (visible when Mode context is active)
  QList<int> mode_ctx = {context_id_};

  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopRight, device_panel_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomCenter, func_bar_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomLeft, coords_label_, 0, mode_ctx);

  // Notification
  QTimer::singleShot(500, [workbench, this]() { workbench->ShowNotification(tr("Welcome to Workspace 1"), 3000); });

  // Update icons
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    UpdateIcons(theme_service->Theme()->Id());
  } else {
    UpdateIcons("light");
  }
}

void Ws1Page::Deactivate() {
  SPDLOG_INFO("Ws1Page::Deactivate called.");
  // Workbench::Clear() is called by ModeManager before next activation.
  // We don't need to do much unless we want to save state.
}

void Ws1Page::setupDefaultUi() {
  // 1. Tree View
  tree_view_ = new QTreeView();
  tree_view_->setHeaderHidden(true);
  setupModel();
  tree_view_->setModel(model_);

  // 2. Background
  bg_label_ = new QLabel(tr("3D Rendering Area (Background)"));
  bg_label_->setObjectName("ws1_bg_label");
  bg_label_->setAlignment(Qt::AlignCenter);

  // 3. Device Panel
  auto* collapsable = new sss::dscore::CollapsibleWidget(tr("Device Info"));
  auto* content_widget = new QWidget();
  auto* info_layout = new QVBoxLayout(content_widget);
  info_layout->setContentsMargins(4, 4, 4, 4);
  info_label_ = new QLabel(tr("Scanner A: Ready"));
  info_layout->addWidget(info_label_);
  info_layout->addWidget(new QLabel(tr("Temp: 45C | FPS: 60")));
  collapsable->SetContentWidget(content_widget);
  device_panel_ = collapsable;

  // 4. Function Bar
  func_bar_ = new QWidget();
  auto* func_layout = new QHBoxLayout(func_bar_);
  enable_button_ = new QPushButton(tr("Enable Context"));
  disable_button_ = new QPushButton(tr("Disable Context"));
  func_layout->addWidget(enable_button_);
  func_layout->addWidget(disable_button_);

  connect(enable_button_, &QPushButton::clicked, this, &Ws1Page::onEnableSubContext);
  connect(disable_button_, &QPushButton::clicked, this, &Ws1Page::onDisableSubContext);

  // 5. Coords
  coords_label_ = new QLabel("X: 100.0 Y: 200.5 Z: 15.3");
  coords_label_->setObjectName("overlay_coords_label");
}

void Ws1Page::UpdateIcons(const QString& /*theme_id*/) {
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service == nullptr) return;

  const QString base_path = ":/ws1/resources/icons";
  if (enable_button_ != nullptr) enable_button_->setIcon(theme_service->GetIcon(base_path, "action_enable.svg"));
  if (disable_button_ != nullptr) disable_button_->setIcon(theme_service->GetIcon(base_path, "action_disable.svg"));
}

void Ws1Page::SetSubContextId(int id) { sub_context_id_ = id; }

void Ws1Page::onEnableSubContext() {  // NOLINT
  SPDLOG_INFO("Ws1Page::onEnableSubContext called");
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->AddActiveContext(sub_context_id_);
  }
}

void Ws1Page::onDisableSubContext() {  // NOLINT
  SPDLOG_INFO("Ws1Page::onDisableSubContext called");
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->RemoveActiveContext(sub_context_id_);
  }
}

void Ws1Page::setupModel() {
  model_ = new QStandardItemModel(this);
  QStandardItem* root_node = model_->invisibleRootItem();
  root_node->appendRow(new QStandardItem(tr("Reference")));
  root_node->appendRow(new QStandardItem(tr("Data")));
  root_node->appendRow(new QStandardItem(tr("Features")));
}

void Ws1Page::retranslateUi() {
  // Update texts...
}

}  // namespace sss::ws1
