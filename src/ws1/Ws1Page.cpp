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
#include "dscore/IWorkbench.h"  // 需要访问全局工作台
#include "extsystem/IComponentManager.h"

namespace sss::ws1 {

Ws1Page::Ws1Page(int context_id, QObject* parent) : sss::dscore::IMode(parent), context_id_(context_id) {
  SPDLOG_INFO("Ws1Page (Mode) constructor called.");

  // 连接到主题服务
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    connect(theme_service, &sss::dscore::IThemeService::ThemeChanged, this, &Ws1Page::UpdateIcons);
  }

  setupDefaultUi();  // 创建一次小部件
}

Ws1Page::~Ws1Page() {
  // 小部件是由我们拥有还是由 Qt 父对象拥有？
  // 如果我们将它们传递给 Workbench，它们可能会被重新指定父对象。
  // 我们应该小心所有权问题。
  // 为了在这个原型中简单起见，我们依赖 QObject 父对象机制，或者如果不小心会有内存泄漏风险。
  // 理想情况下，当调用 Deactivate 时，我们将它们收回或隐藏它们。
  // 但如果 Workbench::Clear() 删除它们，而我们持有指针，我们就会遇到麻烦。
  // 让我们假设 Workbench::Clear() 不会删除小部件，只是移除它们。
  // 我们的 OverlayCanvas::Clear() 将它们从布局中移除并隐藏它们。它不会删除它们，除非它们是
  // 被删除的布局项的子对象？"delete child" 在 OverlayCanvas::Clear 中指的是 QLayoutItem，而不是小部件。所以
  // 我们保留所有权。
  delete tree_view_;
  delete bg_label_;
  delete device_panel_;
  delete func_bar_;
  delete coords_label_;
  delete model_;
}

QString Ws1Page::Id() const { return "ws1.mode"; }
QString Ws1Page::Title() const { return tr("Workspace 1"); }
QIcon Ws1Page::Icon() const {
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if (theme_service != nullptr) {
    return theme_service->GetIcon(":/ws1/resources/icons", "workspace1.svg");
  }
  return {};  // 如果主题服务不可用则返回空图标
}
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
  // 注意：我们可以在这里使用上下文感知的添加。
  // 由于此模式是活动的，其 ContextId 是活动的。
  // 我们可以将小部件绑定到此模式的上下文或子上下文。

  // 对此模式全局（模式上下文活动时可见）
  QList<int> mode_ctx = {context_id_};

  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kTopRight, device_panel_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomCenter, func_bar_, 0, mode_ctx);
  workbench->AddOverlayWidget(sss::dscore::OverlayZone::kBottomLeft, coords_label_, 0, mode_ctx);

  // Notification
  QTimer::singleShot(500, [workbench, this]() { workbench->ShowNotification(tr("Welcome to Workspace 1"), 3000); });

  // 更新图标
  auto* theme_service = sss::extsystem::GetTObject<sss::dscore::IThemeService>();
  if ((theme_service != nullptr) && (theme_service->Theme() != nullptr)) {
    UpdateIcons(theme_service->Theme()->Id());
  } else {
    UpdateIcons("light");
  }
}

void Ws1Page::Deactivate() {
  SPDLOG_INFO("Ws1Page::Deactivate called.");
  // Workbench::Clear() 由 ModeManager 在下次激活前调用。
  // 除非我们想保存状态，否则我们不需要做太多事情。
}

void Ws1Page::setupDefaultUi() {
  // 1. 树视图
  tree_view_ = new QTreeView();
  tree_view_->setHeaderHidden(true);
  setupModel();
  tree_view_->setModel(model_);

  // 2. 背景
  bg_label_ = new QLabel(tr("3D Rendering Area (Background)"));
  bg_label_->setObjectName("ws1_bg_label");
  bg_label_->setAlignment(Qt::AlignCenter);

  // 3. 设备面板
  auto* collapsable = new sss::dscore::CollapsibleWidget(tr("Device Info"));
  auto* content_widget = new QWidget();
  auto* info_layout = new QVBoxLayout(content_widget);
  info_layout->setContentsMargins(4, 4, 4, 4);
  info_label_ = new QLabel(tr("Scanner A: Ready"));
  info_layout->addWidget(info_label_);
  info_layout->addWidget(new QLabel(tr("Temp: 45C | FPS: 60")));
  collapsable->SetContentWidget(content_widget);
  device_panel_ = collapsable;

  // 4. 功能栏
  func_bar_ = new QWidget();
  auto* func_layout = new QHBoxLayout(func_bar_);
  enable_button_ = new QPushButton(tr("Enable Context"));
  disable_button_ = new QPushButton(tr("Disable Context"));
  func_layout->addWidget(enable_button_);
  func_layout->addWidget(disable_button_);

  connect(enable_button_, &QPushButton::clicked, this, &Ws1Page::onEnableSubContext);
  connect(disable_button_, &QPushButton::clicked, this, &Ws1Page::onDisableSubContext);

  // 5. 坐标
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
  // 更新文本...
}

}  // namespace sss::ws1
