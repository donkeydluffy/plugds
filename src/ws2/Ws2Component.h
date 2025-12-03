#pragma once

#include <QObject>
#include <memory>

#include "extsystem/IComponent.h"

namespace sss::dscore {
class ICommandManager;
class ILanguageService;
}  // namespace sss::dscore

namespace sss::ws2 {

class Ws2Page;
class Ws2UIProvider;

class Ws2Component : public QObject, public sss::extsystem::IComponent {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")
  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  Ws2Component() = default;
  ~Ws2Component() override = default;

  // IComponent interface
  void InitialiseEvent() override;
  void InitialisationFinishedEvent() override;
  void FinaliseEvent() override;

 public slots:  // NOLINT
  void UpdateIcons(const QString& theme_id);

 private:
  int page_context_id_ = 0;
  int sub_context_id_ = 0;
  Ws2Page* ws2_mode_ = nullptr;

  std::unique_ptr<Ws2UIProvider> ui_provider_;
};

}  // namespace sss::ws2
