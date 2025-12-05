#pragma once

#include <QObject>
#include <QPointer>

#include "dscore/IMode.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QTreeView;
class QLabel;
class QPushButton;
class QWidget;
QT_END_NAMESPACE

namespace sss::ws1 {

class Ws1Page : public sss::dscore::IMode {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IMode)

 public:
  explicit Ws1Page(int context_id, QObject* parent = nullptr);
  ~Ws1Page() override;

  void SetSubContextId(int id);

  // IMode 接口实现
  [[nodiscard]] QString Id() const override;
  [[nodiscard]] QString Title() const override;
  [[nodiscard]] QIcon Icon() const override;
  [[nodiscard]] int ContextId() const override;
  [[nodiscard]] int Priority() const override;
  void Activate() override;
  void Deactivate() override;

 public slots:  // NOLINT
  void UpdateIcons(const QString& theme_id);

 private Q_SLOTS:
  void onEnableSubContext();
  void onDisableSubContext();

 private:  // NOLINT
  void setupDefaultUi();
  void setupModel();
  void retranslateUi();

  // 小部件（激活时由 Workbench 拥有，但我们持有指针来管理它们）
  // 注意：当调用 Deactivate() 时，我们可能想要隐藏它们或让 Workbench 清除它们。
  // 由于我们想要保持状态（如树展开），我们应该尽可能保持所有权
  // 或重新创建它们。
  // 目前，让我们创建一次并重用它们。
  QPointer<QTreeView> tree_view_;
  QPointer<QStandardItemModel> model_;
  QPointer<QLabel> bg_label_;
  QPointer<QWidget> device_panel_;
  QPointer<QWidget> func_bar_;
  QPointer<QLabel> coords_label_;
  QPointer<QLabel> info_label_;
  QPointer<QLabel> status_label_;
  QPointer<QLabel> squeeze_widget_;

  QPointer<QPushButton> enable_button_;
  QPointer<QPushButton> disable_button_;

  int context_id_ = 0;
  int sub_context_id_ = 0;
};

}  // namespace sss::ws1
