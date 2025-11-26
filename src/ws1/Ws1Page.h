#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeView;
class QStandardItemModel;
class QSplitter;
class QPushButton;
QT_END_NAMESPACE

namespace sss::ws1 {

class Ws1Page : public QWidget {
  Q_OBJECT

 public:
  explicit Ws1Page(QWidget* parent = nullptr);
  ~Ws1Page() override;

  void SetSubContextId(int id);

 private Q_SLOTS:
  void onEnableSubContext();
  void onDisableSubContext();

 private:  // NOLINT
  void setupModel();

  QSplitter* splitter_;
  QTreeView* tree_view_;
  QStandardItemModel* model_;
  QPushButton* enable_button_;
  QPushButton* disable_button_;

  int sub_context_id_ = 0;
};

}  // namespace sss::ws1
