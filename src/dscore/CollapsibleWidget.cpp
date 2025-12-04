#include "dscore/CollapsibleWidget.h"

#include <QPropertyAnimation>
#include <QToolButton>
#include <QVBoxLayout>

namespace sss::dscore {

CollapsibleWidget::CollapsibleWidget(const QString& title, QWidget* parent) : QWidget(parent) {
  main_layout_ = new QVBoxLayout(this);
  main_layout_->setContentsMargins(0, 0, 0, 0);
  main_layout_->setSpacing(0);

  // Header / Toggle Button
  toggle_button_ = new QToolButton(this);
  toggle_button_->setText(title);
  toggle_button_->setCheckable(true);
  toggle_button_->setChecked(true);
  toggle_button_->setStyleSheet(
      "QToolButton { border: none; background-color: rgba(50, 50, 50, 200); color: white; text-align: left; padding: "
      "4px; font-weight: bold; } QToolButton:hover { background-color: rgba(70, 70, 70, 200); }");
  toggle_button_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toggle_button_->setArrowType(Qt::DownArrow);
  toggle_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(toggle_button_, &QToolButton::toggled, this, &CollapsibleWidget::onToggle);

  main_layout_->addWidget(toggle_button_);
}

CollapsibleWidget::~CollapsibleWidget() = default;

void CollapsibleWidget::SetContentWidget(QWidget* widget) {
  if (widget == nullptr) return;

  // 如果有旧内容则移除（简单实现假设只有一个内容）
  if (content_area_ != nullptr) {
    content_area_->hide();
    main_layout_->removeWidget(content_area_);
    content_area_->deleteLater();
  }

  content_area_ = widget;
  content_area_->setVisible(is_expanded_);
  main_layout_->addWidget(content_area_);
}

void CollapsibleWidget::SetTitle(const QString& title) { toggle_button_->setText(title); }

QString CollapsibleWidget::Title() const { return toggle_button_->text(); }

void CollapsibleWidget::Expand() { toggle_button_->setChecked(true); }

void CollapsibleWidget::Collapse() { toggle_button_->setChecked(false); }

bool CollapsibleWidget::IsExpanded() const { return is_expanded_; }

void CollapsibleWidget::onToggle(bool checked) {
  is_expanded_ = checked;
  toggle_button_->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);

  if (content_area_ != nullptr) {
    content_area_->setVisible(checked);
  }

  // Important: Notify parent layout (OverlayCanvas) to adjust size
  updateGeometry();
  if (parentWidget() != nullptr) {
    parentWidget()->adjustSize();
  }
}

}  // namespace sss::dscore
