#include "Ws1Page.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTreeView>

namespace sss::ws1 {

Ws1Page::Ws1Page(QWidget* parent) : QWidget(parent) {
  // Main layout for the page
  auto* main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  // Splitter
  splitter_ = new QSplitter(Qt::Horizontal, this);

  // Left side: Tree View
  tree_view_ = new QTreeView(splitter_);
  tree_view_->header()->setVisible(false);
  setupModel();

  // Right side: Placeholder
  auto* right_pane = new QWidget(splitter_);

  splitter_->addWidget(tree_view_);
  splitter_->addWidget(right_pane);

  // Set initial size ratio 1:4
  splitter_->setSizes({100, 400});

  main_layout->addWidget(splitter_);
  setLayout(main_layout);
}

Ws1Page::~Ws1Page() = default;

void Ws1Page::setupModel() {
  model_ = new QStandardItemModel(this);
  tree_view_->setModel(model_);

  QStandardItem* root_node = model_->invisibleRootItem();

  const QStringList top_level_items = {"参考", "数据", "扫描", "特征", "坐标系", "对齐组", "报告"};

  for (const QString& item_text : top_level_items) {
    auto* top_item = new QStandardItem(item_text);
    top_item->setEditable(false);

    // Add placeholder children
    auto* child1 = new QStandardItem("对象1");
    child1->setEditable(false);
    auto* child2 = new QStandardItem("对象2");
    child2->setEditable(false);

    top_item->appendRow(child1);
    top_item->appendRow(child2);

    root_node->appendRow(top_item);
  }
}

}  // namespace sss::ws1
