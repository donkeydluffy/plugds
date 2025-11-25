#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeView;
class QStandardItemModel;
class QSplitter;
QT_END_NAMESPACE

namespace sss::ws1 {

class Ws1Page : public QWidget {
  Q_OBJECT

 public:
  explicit Ws1Page(QWidget* parent = nullptr);
  ~Ws1Page() override;

 private:
  void setupModel();

  QSplitter* splitter_;
  QTreeView* tree_view_;
  QStandardItemModel* model_;
};

}  // namespace sss::ws1
