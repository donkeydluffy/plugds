#include "DsresourcesComponent.h"

#include <QDebug>

#include "LanguageService.h"
#include "ThemeService.h"
#include "extsystem/IComponentManager.h"

namespace sss::dsresources {

auto DsresourcesComponent::InitialiseEvent() -> void {
  qDebug() << "Initializing DsresourcesComponent";

  // Create services using smart pointers for automatic memory management
  language_service_ = std::make_unique<LanguageService>();
  theme_service_ = std::make_unique<ThemeService>();

  // Register services with the global object registry
  sss::extsystem::AddObject(language_service_.get());
  sss::extsystem::AddObject(theme_service_.get());

  // Register dscore translations here (Inverse Dependency)
  // Since dscore loads before dsresources, it can't register itself.
  // We assume dscore has placed its resources at ":/dscore/i18n"
  language_service_->RegisterTranslator("dscore", ":/dscore/i18n");

  // Load default theme
  theme_service_->LoadTheme("dark");

  qDebug() << "DsresourcesComponent initialized. Services registered.";
}

auto DsresourcesComponent::InitialisationFinishedEvent() -> void {
  // Nothing special to do here for now
}

auto DsresourcesComponent::FinaliseEvent() -> void {
  qDebug() << "Finalising DsresourcesComponent";

  // Remove objects from global registry before smart pointers auto-delete them
  sss::extsystem::RemoveObject(language_service_.get());
  sss::extsystem::RemoveObject(theme_service_.get());

  // Smart pointers will automatically clean up services
  language_service_.reset();
  theme_service_.reset();
}

}  // namespace sss::dsresources