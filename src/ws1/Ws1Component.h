#pragma once

#include <QObject>

#include "extsystem/IComponent.h"

namespace sss::dscore {
class ICommandManager;
class ILanguageService;
class IMenuService;
}  // namespace sss::dscore

namespace sss::ws1 {

class Ws1Page;  // Forward declare the Mode class

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
  /**
   * @brief       Registers menu items using the new IMenuService API.
   *
   * @details     This is the preferred approach for plugins to register menus
   *              without needing to modify MainWindow.
   */
  void registerMenuItems(sss::dscore::IMenuService* menu_service);

  /**
   * @brief       Fallback method using direct ICommandManager API.
   */
  void createSampleCommand(sss::dscore::ICommandManager* command_manager);

  int page_context_id_ = 0;
  int sub_context_id_ = 0;
  Ws1Page* ws1_mode_ = nullptr;  // Keep track of our mode instance
};

}  // namespace sss::ws1
