#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include <memory>
#include <random>

#include "dscore/ICore.h"

namespace sss::dscore {
class MainWindow;
class IPageManager;
class IStatusbarManager;

/**
 * @brief       Core 类是应用程序的根组件。
 *
 * @details     提供 ICore 的实现，为应用程序提供主窗口，并提供应用程序框架（接口），
 *              其他组件使用这些接口来扩展功能。
 * @class       sss::dscore::Core Core.h <Core>
 */
class Core : public sss::dscore::ICore {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICore)

 public:
  /**
   * @brief       构造一个新的 Core 实例。
   */
  Core();

  /**
   * @brief       销毁 Core。
   */
  ~Core() override;

  /**
   * @brief       返回主窗口实例。
   *
   * @details     返回指向主窗口的指针，此函数始终返回相同的
   *              QMainWindow 指针，因此可以被应用程序的任何部分调用以获取
   *              主窗口的句柄。
   *
   * @see         sss::dscore::ICore::GetMainWindow
   *
   * @returns     返回指向 QMainWindow 的指针。
   */
  auto GetMainWindow() -> QMainWindow* override;

  /**
   * @brief       打开核心。
   *
   * @details     应该在组件加载后由应用程序调用一次。组件连接到
   *              sss::dscore::ICore::CoreOpened 信号以执行加载后初始化。
   *
   * @see         sss::dscore::ICore::CoreOpened
   */
  auto Open() -> void;

  /**
   * @brief       提供介于最小值和最大值之间的随机数。
   *
   * @param[in]   minimum_value 随机数的下界。
   * @param[in]   maximum_value 随机数的上界。
   *
   * @returns     介于边界之间的随机数。
   */

  auto Random(int minimum_value, int maximum_value) -> int override;

  /**
   * @brief       返回存储位置。
   *
   * @note        这是应该存储持久数据的文件夹，通常由操作系统提供，
   *              但是，对于应用程序的便携版本，它可能是另一个文件夹。
   *
   * @returns     应该保存数据的文件夹。
   */
  auto StorageFolder() -> QString override;

 private:
  //! @cond

  std::unique_ptr<sss::dscore::MainWindow> main_window_;
  std::unique_ptr<IStatusbarManager> statusbar_manager_;

  std::random_device random_device_;

  std::unique_ptr<std::mt19937> random_generator_;

  //! @endcond
};
}  // namespace sss::dscore
