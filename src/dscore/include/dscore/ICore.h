#pragma once

#include <QDebug>
#include <QMainWindow>
#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

/**
 * @brief       ICore 接口是应用程序的根组件。
 *
 * @details     ICore 应该为应用程序提供主窗口和应用程序的通用框架（接口），
 *              其他组件使用这些接口来扩展功能。
 *
 * @class       sss::dscore::ICore ICore.h <ICore>
 */
class DS_CORE_DLLSPEC ICore : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       返回 sss::dscore::ICore 实例。
   */
  static auto GetInstance() -> ICore* { return sss::extsystem::GetTObject<ICore>(); }

  /**
   * @brief       返回主窗口实例。
   *
   * @details     返回指向主窗口的指针，此函数总是返回相同的
   *              QMainWindow 指针，因此可以由应用程序的任何部分调用以获取
   *              主窗口的句柄。
   *
   * @returns     返回指向 QMainWindow 的指针。
   */
  virtual auto GetMainWindow() -> QMainWindow* = 0;

  /**
   * @brief       此信号在 ICore 初始化后发出。
   *
   * @details     此信号在所有插件加载和初始化后发出，允许
   *              组件在知道所有插件都已初始化的情况下执行后初始化功能。
   */
  Q_SIGNAL void CoreOpened();

  /**
   * @brief       提供最小值和最大值之间的随机数。
   *
   * @param[in]   minimumValue 随机数的下界。
   * @param[in]   maximumValue 随机数的上界。
   *
   * @returns     界限之间的随机数。
   */

  virtual auto Random(int minimum_value, int maximum_value) -> int = 0;

  /**
   * @brief       返回存储位置。
   *
   * @note        这是应该存储持久化数据的文件夹，通常由
   *              操作系统提供，但是对于应用程序的可移植版本，
   *              它可能是另一个文件夹
   *
   * @returns     应该保存数据的文件夹。
   */
  virtual auto StorageFolder() -> QString = 0;

  /**
   * 具有虚函数的类不应具有公共的非虚析构函数：
   */
  ~ICore() override = default;
};

/**
 * @brief       获取主窗口的便捷调用。
 *
 * @returns     如果已创建则返回 QMainWindow 实例；否则返回 nullptr；
 */
inline auto MainWindowInstance() -> QMainWindow* {
  auto* core = ICore::GetInstance();

  if (core != nullptr) return core->GetMainWindow();

  return nullptr;
}
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICore, "sss.dscore.ICore/1.0.0")
