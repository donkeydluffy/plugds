#pragma once

#include <QLocale>
#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief Language Service Interface
 * Responsible for managing system language switching and loading component translation files.
 */
class DS_CORE_DLLSPEC ILanguageService : public QObject {
  Q_OBJECT

 public:
  virtual ~ILanguageService() = default;

  /**
   * @brief Register a component's translation file.
   * Components call this during initialization to tell the service where to find their .qm files.
   *
   * @param component_name Unique identifier for the component (e.g., "ws1", "dscore").
   * @param translation_path Resource path (e.g., ":/ws1/i18n") or file system path.
   */
  virtual auto RegisterTranslator(const QString& component_name, const QString& translation_path) -> void = 0;

  /**
   * @brief Switch the system language.
   * Loads all registered component translation files for the target locale and triggers QEvent::LanguageChange.
   *
   * @param locale Target locale (e.g., QLocale::Chinese).
   */
  virtual auto SwitchLanguage(const QLocale& locale) -> void = 0;

  /**
   * @brief Get the currently active locale.
   * @returns The current QLocale.
   */
  [[nodiscard]] virtual auto GetCurrentLocale() const -> QLocale = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ILanguageService, "sss.dscore.ILanguageService/1.0.0")
