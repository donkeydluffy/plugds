#pragma once
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
class IStatusbarManager;

/**
 * @brief       为应用程序状态栏贡献内容的组件接口。
 *
 * @details     实现此接口来向全局状态栏添加控件或指示器。
 */
class DS_CORE_DLLSPEC IStatusbarProvider {
 public:
  virtual ~IStatusbarProvider() = default;

  /**
   * @brief       为应用程序状态栏做出贡献。
   *
   * @param[in]   statusbar_manager 应用程序的状态栏管理器实例。
   */
  virtual void ContributeToStatusbar(IStatusbarManager* statusbar_manager) = 0;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IStatusbarProvider, "sss.dscore.IStatusbarProvider/1.0")
