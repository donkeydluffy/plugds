#pragma once
#include <QMap>
#include <QTranslator>
#include <memory>
#include <vector>

#include "dscore/ILanguageService.h"

namespace sss::dsresources {

class LanguageService : public sss::dscore::ILanguageService {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ILanguageService)

 public:
  LanguageService();
  ~LanguageService() override;

  auto RegisterTranslator(const QString& component_name, const QString& translation_path) -> void override;
  auto SwitchLanguage(const QLocale& locale) -> void override;
  [[nodiscard]] auto GetCurrentLocale() const -> QLocale override;

 private:
  struct ComponentInfo {
    QString name;
    QString path;
  };
  std::vector<ComponentInfo> components_;  // Changed to QVector for Qt consistency
  QLocale current_locale_;
  std::vector<std::unique_ptr<QTranslator>> active_translators_;  // Changed to QVector for Qt consistency
};

}  // namespace sss::dsresources
