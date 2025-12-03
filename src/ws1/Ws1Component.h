#pragma once

#include <QObject>
#include <memory>

#include "extsystem/IComponent.h"

namespace sss::dscore {
class ICommandManager;
class ILanguageService;
}  // namespace sss::dscore

namespace sss::ws1 {

class Ws1Page;
class Ws1UIProvider;

class Ws1Component : public QObject, public sss::extsystem::IComponent {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")
  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  Ws1Component() = default;
  ~Ws1Component() override = default;

  // IComponent interface
  void InitialiseEvent() override;
  void InitialisationFinishedEvent() override;
  void FinaliseEvent() override;

 public slots:  // NOLINT
  void UpdateIcons(const QString& theme_id);

 private:
  // Removed createSampleCommand and createSwitchCommands methods

  int page_context_id_ = 0;
  int sub_context_id_ = 0;
  Ws1Page* ws1_mode_ = nullptr;  // Keep track of our mode instance

  std::unique_ptr<Ws1UIProvider> ui_provider_;
};

}  // namespace sss::ws1
