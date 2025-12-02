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
    opt.icon = QIcon();  // We draw icon manually
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

      // Translate to just below the icon, centered horizontally
      // icon (32) + padding (4) + gap (4)
      int text_start_y = icon_size + padding + 4;
      p.translate(width() / 2, text_start_y);  // NOLINT
      p.rotate(90);

      // Available height for text
      int available_height = height() - text_start_y - padding;
      QRect text_rect(0, -width() / 2, available_height, width());

      // Elide text if it fits, though rotated elision is tricky.
      // Standard drawText with rotation should handle basic rendering.
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
  layout_->setSpacing(4);  // Reduce spacing for tab-like feel

  // Spacer to push buttons to the top
  layout_->addStretch();

  button_group_ = new QButtonGroup(this);
  button_group_->setExclusive(true);

  connect(button_group_, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this,
          [this](QAbstractButton* button) {
            QString id = button->property("mode_id").toString();
            emit ModeSelected(id);
          });

  // Slightly wider to accommodate visual comfort
  setFixedWidth(50);
}

ModeSwitcher::~ModeSwitcher() = default;

void ModeSwitcher::AddModeButton(const QString& id, const QString& title, const QIcon& icon) {
  auto* btn = new RotatedTabButton(this);

  btn->setText(title);  // Standard text, drawing handles rotation
  btn->setIcon(icon);
  btn->setIconSize(QSize(32, 32));
  btn->setCheckable(true);
  btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Logic handled in paintEvent
  btn->setProperty("mode_id", id);
  btn->setToolTip(title);

  // Use Preferred for vertical to respect sizeHint (content size)
  btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  // Limits
  btn->setMinimumHeight(60);
  btn->setMaximumHeight(160);

  // Add to layout before the stretch item
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
