#include "SplashScreen.h"

#include <QDirIterator>
#include <QFontDatabase>
#include <QPainter>
#include <QtGlobal>

constexpr auto kSplashScreenFilename = ":/app/images/splashscreen.png";
constexpr auto kFontFamily = "Poppins";
constexpr auto kFontSize = 20;

constexpr auto kSplashScreenWidth = 700;
constexpr auto kAlphaLevel = 255.0 * 0.8;
constexpr auto kTextColour = qRgba(0xFF, 0xFF, 0xFF, kAlphaLevel);
constexpr auto kVersionRect = QRectF(45, 123, 210, 32);

sss::SplashScreen::SplashScreen() : QSplashScreen(QPixmap(), Qt::WindowStaysOnTopHint) {
  auto pixmap = QPixmap(kSplashScreenFilename);
  if (pixmap.isNull()) {
    qWarning("Failed to load splash screen image: %s", qUtf8Printable(kSplashScreenFilename));
  }

  // Handle null pixmap to avoid scaling errors
  auto scaled_pixmap = pixmap;
  if (!scaled_pixmap.isNull()) {
    scaled_pixmap = pixmap.scaledToWidth(kSplashScreenWidth * pixmap.devicePixelRatio(), Qt::SmoothTransformation);
  }

  QDirIterator font_dir_iterator(":/app/fonts", QDirIterator::Subdirectories);

  while (font_dir_iterator.hasNext()) {
    font_dir_iterator.next();

    QFontDatabase::addApplicationFont(font_dir_iterator.filePath());
  }

  // Ensure we don't divide by zero if pixmap was null
  scale_factor_ = scaled_pixmap.isNull() ? 1.0f : static_cast<float>(scaled_pixmap.width()) / static_cast<float>(pixmap.width());

  setPixmap(scaled_pixmap);

  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

  show();
}

sss::SplashScreen::~SplashScreen() = default;

auto sss::SplashScreen::GetInstance() -> sss::SplashScreen* {
  static sss::SplashScreen* instance;

  if (instance == nullptr) {
    instance = new sss::SplashScreen;
  }

  return (instance);
}

auto sss::SplashScreen::drawContents(QPainter* painter) -> void {
  auto font = QFont(kFontFamily, kFontSize, QFont::Weight::Bold);
  auto version_text = QString("1.0.0");
  auto text_rect = QRectF(kVersionRect.topLeft() * scale_factor_, kVersionRect.size() * scale_factor_).toRect();

  painter->save();

  painter->setFont(font);

  painter->setPen(QColor::fromRgba(kTextColour));

  QFontMetrics metrics(font);

  auto box_rect = metrics.tightBoundingRect(version_text);

  text_rect.adjust(0, (kVersionRect.height() - box_rect.height()), 0, (kVersionRect.height() - box_rect.height()));

  painter->drawText(kVersionRect, Qt::AlignCenter | Qt::AlignVCenter, version_text);

  painter->restore();
}
