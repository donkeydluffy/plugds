#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class ICommandManager;

/**
 * @brief       为应用程序工具栏贡献内容的组件接口。
 *
 * @details     实现此接口来定义工具栏项目（按钮、分隔符、控件）。
 */
class DS_CORE_DLLSPEC IToolbarProvider {
 public:
  virtual ~IToolbarProvider() = default;

  /**
   * @brief       为应用程序工具栏结构做出贡献。
   *
   * @param[in]   command_manager 应用程序的命令管理器实例。
   */
  virtual void ContributeToToolbar(ICommandManager* command_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IToolbarProvider, "sss.dscore.IToolbarProvider/1.0")
