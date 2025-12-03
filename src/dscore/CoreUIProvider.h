#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "dscore/ICommandProvider.h"
#include "dscore/IMenuProvider.h"
#include "dscore/IToolbarProvider.h"

namespace sss::dscore {

class ICommandManager;

/**
 * @brief       CoreUIProvider provides the standard default UI contributions (menus, commands, toolbars).
 *
 * @details     This class encapsulates the UI contributions that were previously part of CoreComponent
 *              or MainWindow. It implements the standard providers to populate the application with
 *              default File, Settings, Help menus and common commands like Open, Save, etc.
 */
class DS_CORE_DLLSPEC CoreUIProvider : public QObject,
                                       public sss::dscore::ICommandProvider,
                                       public sss::dscore::IMenuProvider,
                                       public sss::dscore::IToolbarProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandProvider sss::dscore::IMenuProvider sss::dscore::IToolbarProvider)

 public:
  explicit CoreUIProvider(QObject* parent = nullptr);
  ~CoreUIProvider() override;

  // ICommandProvider
  void RegisterCommands(sss::dscore::ICommandManager* command_manager) override;

  // IMenuProvider
  void ContributeToMenu(sss::dscore::ICommandManager* command_manager) override;

  // IToolbarProvider
  void ContributeToToolbar(sss::dscore::ICommandManager* command_manager) override;
};

}  // namespace sss::dscore
