#include "LanguageService.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QWidget>
// #include <spdlog/spdlog.h>

namespace sss::dscore {

LanguageService::LanguageService() { current_locale_ = QLocale::system(); }

LanguageService::~LanguageService() = default;

auto LanguageService::RegisterTranslator(const QString& component_name, const QString& translation_path) -> void {
  components_.push_back({component_name, translation_path});
  // 如果我们希望在语言匹配时立即加载，可以在这里执行。
}

auto LanguageService::SwitchLanguage(const QLocale& locale) -> void {
  if (current_locale_ == locale && !active_translators_.empty()) return;

  // 优化：锁定UI更新以防止全局事件分发时出现闪烁
  // 当 QCoreApplication::installTranslator 在所有部件上触发 QEvent::LanguageChange 时。
  QWidgetList top_levels = QApplication::topLevelWidgets();
  for (QWidget* widget : top_levels) {
    if (widget != nullptr) widget->setUpdatesEnabled(false);
  }

  // 卸载旧的翻译器
  for (auto& translator : active_translators_) {
    QCoreApplication::removeTranslator(translator.get());
  }
  active_translators_.clear();

  current_locale_ = locale;

  // 加载新的翻译器
  for (const auto& comp : components_) {
    auto translator = std::make_unique<QTranslator>();
    // 格式：name_lang_country.qm，例如：ws1_zh_CN.qm 或 ws1_zh.qm
    QString filename = QString("%1_%2").arg(comp.name, locale.name());
    qDebug() << "Attempting to load translator:" << filename << "from" << comp.path;

    if (translator->load(filename, comp.path)) {
      qDebug() << "Successfully loaded:" << filename;
      QCoreApplication::installTranslator(translator.get());
      active_translators_.push_back(std::move(translator));
    } else {
      // 尝试回退到仅语言代码（如 ws1_zh.qm）
      QString short_filename = QString("%1_%2").arg(comp.name, locale.name().split('_').first());
      qDebug() << "Attempting fallback:" << short_filename;
      if (translator->load(short_filename, comp.path)) {
        qDebug() << "Successfully loaded fallback:" << short_filename;
        QCoreApplication::installTranslator(translator.get());
        active_translators_.push_back(std::move(translator));
      } else {
        qWarning() << "Failed to load translator:" << comp.name;
      }
    }
  }

  // 重新启用更新
  for (QWidget* widget : top_levels) {
    if (widget != nullptr) widget->setUpdatesEnabled(true);
  }

  // QCoreApplication 会自动发送 QEvent::LanguageChange 事件?
  emit LanguageChanged(current_locale_);
}

auto LanguageService::GetCurrentLocale() const -> QLocale { return current_locale_; }

}  // namespace sss::dscore
