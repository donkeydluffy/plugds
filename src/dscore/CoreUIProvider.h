#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "dscore/ICommandProvider.h"
#include "dscore/IMenuProvider.h"
#include "dscore/IStatusbarProvider.h"
#include "dscore/IToolbarProvider.h"

namespace sss::dscore {

class ICommandManager;

/**
 * @brief       CoreUIProvider 提供标准的默认 UI 贡献（菜单、命令、工具栏）。
 *
 * @details     此类封装了之前属于 CoreComponent 或 MainWindow 的 UI 贡献。
 *              它实现了标准提供者来为应用程序填充默认的文件、设置、帮助菜单
 *              以及打开、保存等常用命令。
 */
class DS_CORE_DLLSPEC CoreUIProvider : public QObject,
                                       public sss::dscore::ICommandProvider,
                                       public sss::dscore::IMenuProvider,
                                       public sss::dscore::IToolbarProvider,
                                       public sss::dscore::IStatusbarProvider {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::ICommandProvider sss::dscore::IMenuProvider sss::dscore::IToolbarProvider
                   sss::dscore::IStatusbarProvider)

 public:
  explicit CoreUIProvider(QObject* parent = nullptr);
  ~CoreUIProvider() override;

  // ICommandProvider
  void RegisterCommands(sss::dscore::ICommandManager* command_manager) override;

  // IMenuProvider
  void ContributeToMenu(sss::dscore::ICommandManager* command_manager) override;

  // IToolbarProvider
  void ContributeToToolbar(sss::dscore::ICommandManager* command_manager) override;

  // IStatusbarProvider
  void ContributeToStatusbar(sss::dscore::IStatusbarManager* statusbar_manager) override;
};

}  // namespace sss::dscore
