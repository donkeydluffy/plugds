#include "ModeSwitcher.h"

#include <QButtonGroup>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>

namespace sss::dscore {

namespace {

class RotatedTabButton : public QToolButton {
 public:
  explicit RotatedTabButton(QWidget* parent = nullptr) : QToolButton(parent) {}

 protected:
  void paintEvent(QPaintEvent* /*event*/) override {
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    // 1. Draw Background/Frame/State (without content)
    QString text = opt.text;
    QIcon icon = opt.icon;
    opt.text.clear();
    opt.icon = QIcon();  // 我们手动绘制图标
    p.drawComplexControl(QStyle::CC_ToolButton, opt);

    // 2. Draw Icon (Top Centered)
    int icon_size = 32;
    int padding = 4;
    QRect icon_rect(0, padding, width(), icon_size);

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    if (mode == QIcon::Normal && ((opt.state & QStyle::State_MouseOver) != 0)) {
      mode = QIcon::Active;
    }
    QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;

    icon.paint(&p, icon_rect, Qt::AlignCenter, mode, state);

    // 3. Draw Text (Rotated 90 degrees Clockwise)
    if (!text.isEmpty()) {
      p.save();

      // 平移到图标正下方，水平居中
      // icon (32) + padding (4) + gap (4)
      int text_start_y = icon_size + padding + 4;
      p.translate(width() / 2, text_start_y);  // NOLINT
      p.rotate(90);

      // 文本可用高度
      int available_height = height() - text_start_y - padding;
      QRect text_rect(0, -width() / 2, available_height, width());

      // 如果文本合适则省略，不过旋转的省略比较复杂。
      // 标准的drawText配合旋转应该能处理基本渲染。
      p.drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, text);

      p.restore();
    }
  }

  [[nodiscard]] QSize sizeHint() const override {
    QSize s = QToolButton::sizeHint();
    QFontMetrics fm(font());
    int text_len = fm.horizontalAdvance(text());

    // Icon (32) + Top Padding (4) + Gap (4) + Text + Bottom Padding (8)
    int h = 32 + 4 + 4 + text_len + 8;
    return {s.width(), h};
  }
};

}  // namespace

ModeSwitcher::ModeSwitcher(QWidget* parent) : QWidget(parent) {
  layout_ = new QVBoxLayout(this);
  layout_->setContentsMargins(2, 2, 2, 2);
  layout_->setSpacing(4);  // 减少间距以获得类似标签页的感觉

  // 弹簧来将按钮推到顶部
  layout_->addStretch();

  button_group_ = new QButtonGroup(this);
  button_group_->setExclusive(true);

  connect(button_group_, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this,
          [this](QAbstractButton* button) {
            QString id = button->property("mode_id").toString();
            emit ModeSelected(id);
          });

  // 稍宽一些以提供视觉舒适感
  setFixedWidth(50);
}

ModeSwitcher::~ModeSwitcher() = default;

void ModeSwitcher::AddModeButton(const QString& id, const QString& title, const QIcon& icon) {
  auto* btn = new RotatedTabButton(this);

  btn->setText(title);  // 标准文本，绘制处理旋转
  btn->setIcon(icon);
  btn->setIconSize(QSize(32, 32));
  btn->setCheckable(true);
  btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // 逻辑在paintEvent中处理
  btn->setProperty("mode_id", id);
  btn->setToolTip(title);

  // Use Preferred for vertical to respect sizeHint (content size)
  btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  // Limits
  btn->setMinimumHeight(60);
  btn->setMaximumHeight(160);

  // 在弹性项目之前添加到布局
  layout_->insertWidget(layout_->count() - 1, btn);

  button_group_->addButton(btn);
}

void ModeSwitcher::SetActiveMode(const QString& id) {
  for (auto* btn : button_group_->buttons()) {
    if (btn->property("mode_id").toString() == id) {
      btn->setChecked(true);
      return;
    }
  }
}

}  // namespace sss::dscore
