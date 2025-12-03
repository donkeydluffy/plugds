#include "Ws2Page.h"

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
#include "dscore/IThemeService.h"
#include "dscore/IWorkbench.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws2 {

Ws2Page::Ws2Page(int context_id, QObject* parent) : sss::dscore::IMode(parent), context_id_(context_id) {
  SPDLOG_INFO("Ws2Page (Mode) constructor called.");

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws2Page::UpdateIcons);
  }

  setupDefaultUi();
}

Ws2Page::~Ws2Page() {
  delete tree_view_;
  delete bg_label_;
  delete device_panel_;
  delete func_bar_;
  delete coords_label_;
  delete model_;
}

QString Ws2Page::Id() const { return "ws2.mode"; }
QString Ws2Page::Title() const { return tr("Workspace 2"); }
QIcon Ws2Page::Icon() const {
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    return theme_service->GetIcon(":/ws2/resources/icons", "workspace2.svg");
  }
  return {};
}
int Ws2Page::ContextId() const { return context_id_; }
int Ws2Page::Priority() const { return 20; }  // Higher priority or just different order

void Ws2Page::Activate() {
  SPDLOG_INFO("Ws2Page::Activate called.");
  auto* workbench = sss::extsystem::GetTObject<sss::dscore::IWorkbench>();
  if (workbench == nullptr) {
    SPDLOG_ERROR("Global IWorkbench not found!");
    return;
  }

  // 1. Left Sidebar
  workbench->AddSidePanel("ws2.sidebar.tree", tree_view_, tr("Project Explorer"), QIcon{});

  // 2. Background
  workbench->SetBackgroundWidget(bg_label_);

  // 3. Overlays
  QList<int> mode_ctx = {context_id_};

  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopLeft, device_panel_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomRight, func_bar_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopCenter, coords_label_, 0, mode_ctx);

  QTimer::singleShot(500, [workbench, this]() { workbench->ShowNotification(tr("Welcome to Workspace 2"), 3000); });

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    UpdateIcons(theme_service->Theme()->Id());
  } else {
    UpdateIcons("light");
  }
}

void Ws2Page::Deactivate() { SPDLOG_INFO("Ws2Page::Deactivate called."); }

void Ws2Page::setupDefaultUi() {
  // 1. Tree View
  tree_view_ = new QTreeView();
  tree_view_->setHeaderHidden(true);
  setupModel();
  tree_view_->setModel(model_);

  // 2. Background
  bg_label_ = new QLabel(tr("Workspace 2 Area (Background)"));
  bg_label_->setObjectName("ws2_bg_label");
  bg_label_->setAlignment(Qt::AlignCenter);

  // 3. Device Panel
  auto* collapsable = new sss::dscore::CollapsibleWidget(tr("System Status"));
  auto* content_widget = new QWidget();
  auto* info_layout = new QVBoxLayout(content_widget);
  info_layout->setContentsMargins(4, 4, 4, 4);
  info_label_ = new QLabel(tr("Scanner B: Idle"));
  info_layout->addWidget(info_label_);
  info_layout->addWidget(new QLabel(tr("Memory: 45% | CPU: 12%")));
  collapsable->SetContentWidget(content_widget);
  device_panel_ = collapsable;

  // 4. Function Bar
  func_bar_ = new QWidget();
  auto* func_layout = new QHBoxLayout(func_bar_);
  enable_button_ = new QPushButton(tr("Start Process"));
  disable_button_ = new QPushButton(tr("Stop Process"));
  func_layout->addWidget(enable_button_);
  func_layout->addWidget(disable_button_);

  connect(enable_button_, &QPushButton::clicked, this, &Ws2Page::onEnableSubContext);
  connect(disable_button_, &QPushButton::clicked, this, &Ws2Page::onDisableSubContext);

  // 5. Coords
  coords_label_ = new QLabel("Active Item: None");
  coords_label_->setObjectName("overlay_status_label");
}

void Ws2Page::UpdateIcons(const QString& /*theme_id*/) {
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service == nullptr) return;

  const QString base_path = ":/ws2/resources/icons";
  if (enable_button_ != nullptr) enable_button_->setIcon(theme_service->GetIcon(base_path, "action_play.svg"));
  if (disable_button_ != nullptr) disable_button_->setIcon(theme_service->GetIcon(base_path, "action_stop.svg"));
}

void Ws2Page::SetSubContextId(int id) { sub_context_id_ = id; }

void Ws2Page::onEnableSubContext() const {
  SPDLOG_INFO("Ws2Page::onEnableSubContext called");
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->AddActiveContext(sub_context_id_);
  }
}

void Ws2Page::onDisableSubContext() const {
  SPDLOG_INFO("Ws2Page::onDisableSubContext called");
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->RemoveActiveContext(sub_context_id_);
  }
}

void Ws2Page::setupModel() {
  model_ = new QStandardItemModel(this);
  QStandardItem* root_node = model_->invisibleRootItem();
  root_node->appendRow(new QStandardItem(tr("Inputs")));
  root_node->appendRow(new QStandardItem(tr("Outputs")));
  root_node->appendRow(new QStandardItem(tr("Logs")));
}

void Ws2Page::retranslateUi() {
  // Update texts...
}

}  // namespace sss::ws2
