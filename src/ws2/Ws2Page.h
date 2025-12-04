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

namespace sss::ws2 {

class Ws2Page : public sss::dscore::IMode {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IMode)

 public:
  explicit Ws2Page(int context_id, QObject* parent = nullptr);
  ~Ws2Page() override;

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
  void onEnableSubContext() const;
  void onDisableSubContext() const;

 private:  // NOLINT
  void setupDefaultUi();
  void setupModel();
  void retranslateUi();

  QPointer<QTreeView> tree_view_;
  QPointer<QStandardItemModel> model_;
  QPointer<QLabel> bg_label_;
  QPointer<QWidget> device_panel_;
  QPointer<QWidget> func_bar_;
  QPointer<QLabel> coords_label_;
  QPointer<QLabel> info_label_;
  QPointer<QLabel> status_label_;

  QPointer<QPushButton> enable_button_;
  QPointer<QPushButton> disable_button_;

  int context_id_ = 0;
  int sub_context_id_ = 0;
};

}  // namespace sss::ws2
