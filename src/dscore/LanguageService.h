#pragma once
#include <QMap>
#include <QTranslator>
#include <memory>
#include <vector>

#include "dscore/ILanguageService.h"

namespace sss::dscore {

class LanguageService : public ILanguageService {
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
  std::vector<ComponentInfo> components_;
  QLocale current_locale_;
  std::vector<std::unique_ptr<QTranslator>> active_translators_;
};

}  // namespace sss::dscore
