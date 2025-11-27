#pragma once
#include <QObject>
#include <memory>

#include "extsystem/IComponent.h"

namespace sss::dsresources {

// Forward declarations
class LanguageService;
class ThemeService;

class DsresourcesComponent : public QObject, public sss::extsystem::IComponent {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "sss.dsresources" FILE "metadata.json")
  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  DsresourcesComponent() = default;
  ~DsresourcesComponent() override = default;

  auto InitialiseEvent() -> void override;
  auto InitialisationFinishedEvent() -> void override;
  auto FinaliseEvent() -> void override;

 private:
  std::unique_ptr<LanguageService> language_service_;
  std::unique_ptr<ThemeService> theme_service_;
};

}  // namespace sss::dsresources
