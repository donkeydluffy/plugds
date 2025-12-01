#pragma once

#include <QObject>

#include "dscore/IPageProvider.h"
#include "extsystem/IComponent.h"

namespace sss::dscore {
class ICommandManager;
class ILanguageService;
}  // namespace sss::dscore

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
  [[nodiscard]] auto PageContextId() const -> int override;

 public slots:  // NOLINT
  void UpdateIcons(const QString& theme_id);

 private:
  void createSampleCommand(sss::dscore::ICommandManager* command_manager);
  void createSwitchCommands(sss::dscore::ICommandManager* command_manager, sss::dscore::ILanguageService* lang_service);

  int page_context_id_ = 0;
  int sub_context_id_ = 0;
};

}  // namespace sss::ws1
