#pragma once

#include <QWidget>

#include "dscore/CoreSpec.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QVBoxLayout;
class QParallelAnimationGroup;
class QScrollArea;
class QToolButton;
QT_END_NAMESPACE

namespace sss::dscore {

/**
 * @brief A widget that has a header (toggle button) and a content area that can be collapsed/expanded.
 */
class DS_CORE_DLLSPEC CollapsibleWidget : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString title READ Title WRITE SetTitle)

 public:
  explicit CollapsibleWidget(const QString& title, QWidget* parent = nullptr);
  ~CollapsibleWidget() override;

  void SetContentWidget(QWidget* widget);
  void SetTitle(const QString& title);
  [[nodiscard]] QString Title() const;

  void Expand();
  void Collapse();
  [[nodiscard]] bool IsExpanded() const;

 private slots:
  void onToggle(bool checked);

 private:  // NOLINT
  QToolButton* toggle_button_ = nullptr;
  QWidget* content_area_ = nullptr;
  QVBoxLayout* main_layout_ = nullptr;
  bool is_expanded_ = true;
};

}  // namespace sss::dscore
