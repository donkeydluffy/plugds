#include "SystemTrayIcon.h"

#include <QGuiApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

constexpr auto kDefaultTrayPixmap = ":/app/images/appicon/mono/appicon-1024x1024@2x.png";

sss::dscore::SystemTrayIcon::SystemTrayIcon(QObject* parent)
    : base_pixmap_(QPixmap(kDefaultTrayPixmap)), context_menu_(nullptr) {
  if (base_pixmap_.isNull()) {
    qWarning("Failed to load default tray icon: %s", qUtf8Printable(kDefaultTrayPixmap));
  }
  CreateIcon();
}

sss::dscore::SystemTrayIcon::SystemTrayIcon(const QPixmap& pixmap, QObject* parent) : base_pixmap_(pixmap) {
  CreateIcon();
}

auto sss::dscore::SystemTrayIcon::CreateIcon() -> void {
  systemTray_icon_ = new QSystemTrayIcon;

  context_menu_ = new QMenu;

  auto* quit_action = new QAction(QString(tr("Quit %1")).arg(qAppName()));

  connect(quit_action, &QAction::triggered, [=]() { QGuiApplication::quit(); });

  context_menu_->insertAction(nullptr, quit_action);

  // Set the icon only if it's not null
  if (!base_pixmap_.isNull()) {
    systemTray_icon_->setIcon(QIcon(base_pixmap_));
  }
  
  systemTray_icon_->setContextMenu(context_menu_);
  systemTray_icon_->show();

  connect(systemTray_icon_, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
      Q_EMIT Clicked(sss::dscore::ISystemTrayIcon::MouseButton::kLeft);
    } else if (reason == QSystemTrayIcon::Context) {
      Q_EMIT Clicked(sss::dscore::ISystemTrayIcon::MouseButton::kRight);
    }
  });
}

sss::dscore::SystemTrayIcon::~SystemTrayIcon() {
#if !defined(Q_OS_MACOS)
  delete systemTray_icon_;
#endif
  delete context_menu_;
}

auto sss::dscore::SystemTrayIcon::Geometry() -> QRect { return systemTray_icon_->geometry(); }

auto sss::dscore::SystemTrayIcon::SetVisible(bool visible) -> void {
  visible_ = visible;

  if (visible_) {
    systemTray_icon_->show();
  } else {
    systemTray_icon_->hide();
  }
}

auto sss::dscore::SystemTrayIcon::SetColour(const QColor& new_colour) -> void {
  // Only proceed if base_pixmap_ is not null
  if (base_pixmap_.isNull()) {
    qWarning("Failed to set tray icon colour: base pixmap is null");
    return;
  }

  QImage temporary_image = base_pixmap_.toImage();

  for (int y = 0; y < temporary_image.height(); y++) {
    for (int x = 0; x < temporary_image.width(); x++) {
      auto pixel_colour = temporary_image.pixelColor(x, y);

      pixel_colour.setRedF((pixel_colour.redF() + new_colour.redF()) / 2.0);
      pixel_colour.setGreenF((pixel_colour.greenF() + new_colour.greenF()) / 2.0);
      pixel_colour.setBlueF((pixel_colour.blueF() + new_colour.blueF()) / 2.0);

      pixel_colour.setAlphaF(pixel_colour.alphaF() * new_colour.alphaF());

      temporary_image.setPixelColor(x, y, pixel_colour);
    }
  }

  systemTray_icon_->setIcon(QIcon(QPixmap::fromImage(temporary_image)));
}
