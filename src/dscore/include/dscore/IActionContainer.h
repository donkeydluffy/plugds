#pragma once

#include <QObject>
#include <QWidget>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class ICommand;

/**
 * @brief       定义动作容器类型的枚举。
 */
enum class ContainerType : uint8_t { kMenuBar, kMenu, kToolBar, kRibbonBar, kUnknown };

/**
 * @brief       IActionContainer 接口描述了 QAction 的容器。
 *
 * @details     表示动作的UI容器（菜单、工具栏等）。
 *              抽象了底层的Qt控件，允许对菜单和工具栏进行通用处理。
 *
 * @class       sss::dscore::IActionContainer IActionContainer.h <IActionContainer>
 */
class DS_CORE_DLLSPEC IActionContainer : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       返回此容器的底层 QWidget。
   *
   * @returns     控件实例，如果不适用则返回 nullptr。
   */
  virtual auto GetWidget() -> QWidget* = 0;

  /**
   * @brief       返回容器的类型。
   *
   * @returns     容器的 sss::dscore::ContainerType 类型。
   */
  virtual auto GetType() -> sss::dscore::ContainerType = 0;

  /**
   * @brief       使用特定顺序向容器插入新组。
   *
   * @details     创建命名的分组部分。
   *
   * @param[in]   group_identifier 组的标识符。
   * @param[in]   order 显示顺序/优先级（较低值表示更早/更靠左）。默认为 0。
   */
  virtual auto InsertGroup(QString group_identifier, int order) -> void = 0;

  /**
   * @brief       将命令附加到组的末尾。
   *
   * @param[in]   command 要添加的命令。
   * @param[in]   group_identifier 组的标识符。
   */
  virtual auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void = 0;

  /**
   * @brief       将命令附加到组的末尾。
   *
   * @param[in]   command_identifier 命令的标识符。
   * @param[in]   group_identifier 组的标识符。
   */
  virtual auto AppendCommand(QString command_identifier, QString group_identifier) -> void = 0;

  // 具有虚函数的类不应有公共的虚析构函数：
  ~IActionContainer() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IActionContainer, "sss.dscore.IActionContainer/1.0.0")
