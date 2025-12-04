#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       为应用程序菜单贡献内容的组件接口。
 *
 * @details     实现此接口来定义菜单结构（菜单、组）并将注册的命令放置到其中。
 */
class DS_CORE_DLLSPEC IMenuProvider {
 public:
  virtual ~IMenuProvider() = default;

  /**
   * @brief       为应用程序菜单结构做出贡献。
   *
   * @param[in]   command_manager 应用程序的命令管理器实例，用于查找容器和命令。
   */
  virtual void ContributeToMenu(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMenuProvider, "sss.dscore.IMenuProvider/1.0")
