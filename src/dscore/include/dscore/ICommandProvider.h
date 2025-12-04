#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       为应用程序提供命令/动作的组件接口。
 *
 * @details     实现此接口来向CommandManager注册逻辑命令（动作）。
 *              这将命令的定义与其在菜单或工具栏中的位置分离开来。
 */
class DS_CORE_DLLSPEC ICommandProvider {
 public:
  virtual ~ICommandProvider() = default;

  /**
   * @brief       向命令管理器注册命令。
   *
   * @param[in]   command_manager 应用程序的命令管理器实例。
   */
  virtual void RegisterCommands(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommandProvider, "sss.dscore.ICommandProvider/1.0")
