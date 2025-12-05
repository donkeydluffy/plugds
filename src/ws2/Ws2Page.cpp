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
#include "dscore/ILanguageService.h"
#include "dscore/IThemeService.h"
#include "dscore/IWorkbench.h"
#include "extsystem/IComponentManager.h"
#include "ws2/Ws2Strings.h"

namespace sss::ws2 {

Ws2Page::Ws2Page(int context_id, QObject* parent) : sss::dscore::IMode(parent), context_id_(context_id) {
  SPDLOG_INFO("Ws2Page (Mode) constructor called.");

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws2Page::UpdateIcons);
  }

  auto* lang_service = sss::extsystem::GetTObject<sss::dscore::ILanguageService>();
  if (lang_service != nullptr) {
    connect(lang_service, &sss::dscore::ILanguageService::LanguageChanged, this,
            [this](const QLocale&) { retranslateUi(); });
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
QString Ws2Page::Title() const { return Ws2Strings::WorkspaceTitle(); }
QIcon Ws2Page::Icon() const {
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    return theme_service->GetIcon(":/ws2/resources/icons", "workspace2.svg");
  }
  return {};
}
int Ws2Page::ContextId() const { return context_id_; }
int Ws2Page::Priority() const { return 20; }  // 更高优先级或只是不同顺序

void Ws2Page::Activate() {
  auto* workbench = sss::extsystem::GetTObject<sss::dscore::IWorkbench>();
  if (workbench == nullptr) {
    SPDLOG_ERROR("Global IWorkbench not found!");
    return;
  }

  // 1. 左侧边栏
  workbench->AddSidePanel("ws2.sidebar.tree", tree_view_, Ws2Strings::ProjectBrowser(), QIcon{});

  // 2. 背景
  workbench->SetBackgroundWidget(bg_label_);

  // 3. 覆盖层
  QList<int> mode_ctx = {context_id_};

  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopLeft, device_panel_, 0, mode_ctx, {});
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomRight, func_bar_, 0, mode_ctx, {});
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopCenter, coords_label_, 0, mode_ctx, {});

  QTimer::singleShot(500, [workbench, this]() { workbench->ShowNotification(Ws2Strings::WelcomeMessage(), 3000); });

  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    UpdateIcons(theme_service->Theme()->Id());
  } else {
    UpdateIcons("light");
  }
}

void Ws2Page::Deactivate() { SPDLOG_DEBUG("Ws2Page::Deactivate called."); }

void Ws2Page::setupDefaultUi() {
  // 1. 树视图
  tree_view_ = new QTreeView();
  tree_view_->setHeaderHidden(true);
  setupModel();
  tree_view_->setModel(model_);

  // 2. 背景
  bg_label_ = new QLabel(Ws2Strings::RenderingArea());
  bg_label_->setObjectName("ws2_bg_label");
  bg_label_->setAlignment(Qt::AlignCenter);

  // 3. 设备面板
  auto* collapsable = new sss::dscore::CollapsibleWidget(Ws2Strings::SystemStatus());
  auto* content_widget = new QWidget();
  auto* info_layout = new QVBoxLayout(content_widget);
  info_layout->setContentsMargins(4, 4, 4, 4);
  info_label_ = new QLabel(Ws2Strings::ScannerIdle());
  info_layout->addWidget(info_label_);
  status_label_ = new QLabel(Ws2Strings::StatusInfo());
  info_layout->addWidget(status_label_);
  collapsable->SetContentWidget(content_widget);
  device_panel_ = collapsable;

  // 4. 功能栏
  func_bar_ = new QWidget();
  auto* func_layout = new QHBoxLayout(func_bar_);
  enable_button_ = new QPushButton(Ws2Strings::StartProcess());
  disable_button_ = new QPushButton(Ws2Strings::StopProcess());
  func_layout->addWidget(enable_button_);
  func_layout->addWidget(disable_button_);

  connect(enable_button_, &QPushButton::clicked, this, &Ws2Page::onEnableSubContext);
  connect(disable_button_, &QPushButton::clicked, this, &Ws2Page::onDisableSubContext);

  // 5. 坐标
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
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->AddActiveContext(sub_context_id_);
  }
}

void Ws2Page::onDisableSubContext() const {
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if ((context_manager != nullptr) && sub_context_id_ != 0) {
    context_manager->RemoveActiveContext(sub_context_id_);
  }
}

void Ws2Page::setupModel() {
  model_ = new QStandardItemModel(this);
  QStandardItem* root_node = model_->invisibleRootItem();
  root_node->appendRow(new QStandardItem(Ws2Strings::Input()));
  root_node->appendRow(new QStandardItem(Ws2Strings::Output()));
  root_node->appendRow(new QStandardItem(Ws2Strings::Logs()));
}

void Ws2Page::retranslateUi() {
  if (tree_view_ != nullptr) {
    delete model_;
    setupModel();
    tree_view_->setModel(model_);
  }

  if (bg_label_ != nullptr) bg_label_->setText(Ws2Strings::RenderingArea());

  if ((device_panel_ != nullptr) && (qobject_cast<sss::dscore::CollapsibleWidget*>(device_panel_) != nullptr)) {
    auto* cw = qobject_cast<sss::dscore::CollapsibleWidget*>(device_panel_);
    cw->SetTitle(Ws2Strings::SystemStatus());
  }

  if (info_label_ != nullptr) info_label_->setText(Ws2Strings::ScannerIdle());
  if (status_label_ != nullptr) status_label_->setText(Ws2Strings::StatusInfo());

  if (enable_button_ != nullptr) enable_button_->setText(Ws2Strings::StartProcess());
  if (disable_button_ != nullptr) disable_button_->setText(Ws2Strings::StopProcess());
}

}  // namespace sss::ws2
