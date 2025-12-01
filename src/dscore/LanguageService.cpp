#include "LanguageService.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QWidget>
// #include <spdlog/spdlog.h> // Removed as spdlog is for application logging, not library utility

namespace sss::dscore {

LanguageService::LanguageService() { current_locale_ = QLocale::system(); }

LanguageService::~LanguageService() = default;

auto LanguageService::RegisterTranslator(const QString& component_name, const QString& translation_path) -> void {
  components_.push_back({component_name, translation_path});
  // If we wanted to load immediately if the language matches, we could do it here.
}

auto LanguageService::SwitchLanguage(const QLocale& locale) -> void {
  if (current_locale_ == locale && !active_translators_.empty()) return;

  // OPTIMIZATION: Lock UI updates to prevent flickering during global event dispatch
  // when QCoreApplication::installTranslator triggers QEvent::LanguageChange on all widgets.
  QWidgetList top_levels = QApplication::topLevelWidgets();
  for (QWidget* widget : top_levels) {
    if (widget != nullptr) widget->setUpdatesEnabled(false);
  }

  // Unload old translators
  for (auto& translator : active_translators_) {
    QCoreApplication::removeTranslator(translator.get());
  }
  active_translators_.clear();

  current_locale_ = locale;

  // Load new ones
  for (const auto& comp : components_) {
    auto translator = std::make_unique<QTranslator>();
    // Format: name_lang_country.qm, e.g., ws1_zh_CN.qm or ws1_zh.qm
    QString filename = QString("%1_%2").arg(comp.name, locale.name());
    qDebug() << "Trying to load translator:" << filename << "from" << comp.path;

    if (translator->load(filename, comp.path)) {
      qDebug() << "Loaded successfully:" << filename;
      QCoreApplication::installTranslator(translator.get());
      active_translators_.push_back(std::move(translator));
    } else {
      // Try fallback to just language code (ws1_zh.qm)
      QString short_filename = QString("%1_%2").arg(comp.name, locale.name().split('_').first());
      qDebug() << "Trying fallback:" << short_filename;
      if (translator->load(short_filename, comp.path)) {
        qDebug() << "Loaded fallback successfully:" << short_filename;
        QCoreApplication::installTranslator(translator.get());
        active_translators_.push_back(std::move(translator));
      } else {
        qWarning() << "Failed to load translator for" << comp.name;
      }
    }
  }

  // Re-enable updates
  for (QWidget* widget : top_levels) {
    if (widget != nullptr) widget->setUpdatesEnabled(true);
  }

  // QCoreApplication automatically sends QEvent::LanguageChange
}

auto LanguageService::GetCurrentLocale() const -> QLocale { return current_locale_; }

}  // namespace sss::dscore
