#include "Ws1Page.h"

#include <spdlog/spdlog.h>

#include <QEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

#include "dscore/IContextManager.h"

namespace sss::ws1 {

Ws1Page::Ws1Page(QWidget* parent) : QWidget(parent) {
  SPDLOG_INFO("Ws1Page constructor called.");
  // Main layout for the page
  auto* main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  // Splitter
  splitter_ = new QSplitter(Qt::Horizontal, this);

  // Left side: Tree View
  tree_view_ = new QTreeView(splitter_);
  tree_view_->header()->setVisible(false);
  setupModel();

  // Right side: Control Pane
  auto* right_pane = new QWidget(splitter_);
  auto* right_layout = new QVBoxLayout(right_pane);

  info_label_ = new QLabel(right_pane);
  info_label_->setWordWrap(true);

  enable_button_ = new QPushButton(right_pane);
  disable_button_ = new QPushButton(right_pane);

  right_layout->addWidget(info_label_);
  right_layout->addWidget(enable_button_);
  right_layout->addWidget(disable_button_);
  right_layout->addStretch();
  right_pane->setLayout(right_layout);

  splitter_->addWidget(tree_view_);
  splitter_->addWidget(right_pane);

  // Set initial size ratio 1:4
  splitter_->setSizes({200, 400});

  main_layout->addWidget(splitter_);
  setLayout(main_layout);

  connect(enable_button_, &QPushButton::clicked, this, &Ws1Page::onEnableSubContext);
  connect(disable_button_, &QPushButton::clicked, this, &Ws1Page::onDisableSubContext);

  // Initial translation
  retranslateUi();
}

Ws1Page::~Ws1Page() = default;

void Ws1Page::SetSubContextId(int id) {
  sub_context_id_ = id;
  SPDLOG_INFO("Ws1Page::SetSubContextId called with id: {}", sub_context_id_);
}

void Ws1Page::changeEvent(QEvent* event) {
  if (event->type() == QEvent::LanguageChange) {
    retranslateUi();
  }
  QWidget::changeEvent(event);
}

void Ws1Page::retranslateUi() {
  info_label_->setText(
      tr("Click buttons to change the sub-context.\nWatch the 'Ws1 Sample Command' in the File menu and toolbar."));

  enable_button_->setText(tr("Enable Sub-Context (Activate Command)"));
  disable_button_->setText(tr("Disable Sub-Context (Deactivate Command)"));

  // Rebuild the model to apply new translations
  if (model_ != nullptr) {
    delete model_;
    model_ = nullptr;
  }
  setupModel();
}

void Ws1Page::onEnableSubContext() {  // NOLINT
  SPDLOG_INFO("Ws1Page::onEnableSubContext called, sub_context_id_: {}", sub_context_id_);
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    SPDLOG_ERROR("Ws1Page::onEnableSubContext: failed to get ContextManager");
    return;
  }
  if (sub_context_id_ == 0) {
    SPDLOG_ERROR("Ws1Page::onEnableSubContext: sub_context_id_ is 0");
    return;
  }
  context_manager->AddActiveContext(sub_context_id_);
}

void Ws1Page::onDisableSubContext() {  // NOLINT
  SPDLOG_INFO("Ws1Page::onDisableSubContext called, sub_context_id_: {}", sub_context_id_);
  auto* context_manager = sss::dscore::IContextManager::GetInstance();
  if (context_manager == nullptr) {
    SPDLOG_ERROR("Ws1Page::onDisableSubContext: failed to get ContextManager");
    return;
  }
  if (sub_context_id_ == 0) {
    SPDLOG_ERROR("Ws1Page::onDisableSubContext: sub_context_id_ is 0");
    return;
  }
  context_manager->RemoveActiveContext(sub_context_id_);
}

void Ws1Page::setupModel() {
  model_ = new QStandardItemModel(this);
  tree_view_->setModel(model_);

  QStandardItem* root_node = model_->invisibleRootItem();

  // Note: These strings are hardcoded here. In a real app, you'd want to reload them in retranslateUi.
  // For demonstration, we'll just leave them or use tr() once.
  const QStringList top_level_items = {
      tr("Reference"),       tr("Data"),  tr("Scan"), tr("Features"), tr("Coordinate System"),
      tr("Alignment Group"), tr("Report")};

  for (const QString& item_text : top_level_items) {
    auto* top_item = new QStandardItem(item_text);
    top_item->setEditable(false);

    // Add placeholder children
    auto* child1 = new QStandardItem(tr("Object 1"));
    child1->setEditable(false);
    auto* child2 = new QStandardItem(tr("Object 2"));
    child2->setEditable(false);

    top_item->appendRow(child1);
    top_item->appendRow(child2);

    root_node->appendRow(top_item);
  }
}

}  // namespace sss::ws1
