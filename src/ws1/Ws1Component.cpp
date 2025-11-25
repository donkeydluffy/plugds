#include "Ws1Component.h"

#include <spdlog/spdlog.h>

#include "Ws1Page.h"
#include "dscore/IPageManager.h"
#include "extsystem/IComponentManager.h"

namespace sss::ws1 {

void Ws1Component::InitialiseEvent() {
  SPDLOG_INFO("Ws1Component::InitialiseEvent");
  auto* page_manager = sss::extsystem::GetTObject<sss::dscore::IPageManager>();
  if (page_manager != nullptr) {
    page_manager->AddPage(this);
    SPDLOG_INFO("Ws1Page added to PageManager.");
  } else {
    SPDLOG_ERROR("Failed to get IPageManager service.");
  }
}

void Ws1Component::InitialisationFinishedEvent() {
  // Do nothing
}

void Ws1Component::FinaliseEvent() {
  SPDLOG_INFO("Ws1Component::FinaliseEvent");
  auto* page_manager = sss::extsystem::GetTObject<sss::dscore::IPageManager>();
  if (page_manager != nullptr) {
    page_manager->RemovePage(this);
    SPDLOG_INFO("Ws1Page removed from PageManager.");
  } else {
    SPDLOG_ERROR("Failed to get IPageManager service during finalisation.");
  }
}

auto Ws1Component::CreatePage(QWidget* parent) -> QWidget* { return new Ws1Page(parent); }

auto Ws1Component::PageTitle() const -> QString { return tr("工作区1"); }

}  // namespace sss::ws1
