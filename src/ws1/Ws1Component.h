#pragma once

#include <QObject>

#include "dscore/IPageProvider.h"
#include "extsystem/IComponent.h"

namespace sss::ws1 {

class Ws1Component : public sss::dscore::IPageProvider, public sss::extsystem::IComponent {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")
  Q_INTERFACES(sss::extsystem::IComponent sss::dscore::IPageProvider)

 public:
  Ws1Component() = default;
  ~Ws1Component() override = default;

  // IComponent interface
  void InitialiseEvent() override;
  void InitialisationFinishedEvent() override;
  void FinaliseEvent() override;

  // IPageProvider interface
  auto CreatePage(QWidget* parent) -> QWidget* override;
  [[nodiscard]] auto PageTitle() const -> QString override;
};

}  // namespace sss::ws1
