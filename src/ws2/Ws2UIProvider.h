#pragma once

#include <QObject>

#include "dscore/ICommandProvider.h"
#include "dscore/IMenuProvider.h"
#include "dscore/IToolbarProvider.h"

namespace sss::ws2 {

class Ws2Component;

class Ws2UIProvider : public QObject,
                      public sss::dscore::ICommandProvider,
                      public sss::dscore::IMenuProvider,
                      public sss::dscore::IToolbarProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandProvider sss::dscore::IMenuProvider sss::dscore::IToolbarProvider)

 public:
  explicit Ws2UIProvider(Ws2Component* component, int page_context_id, int sub_context_id);
  ~Ws2UIProvider() override = default;

  // ICommandProvider 接口
  void RegisterCommands(sss::dscore::ICommandManager* command_manager) override;

  // IMenuProvider 接口
  void ContributeToMenu(sss::dscore::ICommandManager* command_manager) override;

  // IToolbarProvider 接口
  void ContributeToToolbar(sss::dscore::ICommandManager* command_manager) override;

 private:
  Ws2Component* component_;
  int page_context_id_;
  int sub_context_id_;
};

}  // namespace sss::ws2
