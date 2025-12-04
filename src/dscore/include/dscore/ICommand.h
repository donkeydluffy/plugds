#pragma once

#include <QAbstractButton>
#include <QAction>
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
/**
 * @brief       ICommand 接口
 *
 * @details     ICommand 代表系统中的可执行命令，命令绑定到给定上下文的 QAction，
 *              这允许命令的目标根据应用程序当前上下文而改变。
 *
 * @class       sss::dscore::ICommand ICommand.h <ICommand>
 */
class DS_CORE_DLLSPEC ICommand : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       返回代理动作。
   *
   * @returns     代理动作
   */
  virtual auto Action() -> QAction* = 0;

  /**
   * @brief       设置命令的活动状态。
   *
   * @param[in]   state 如果活动则为 true；否则为 false。
   */
  virtual auto SetActive(bool state) -> void = 0;

  /**
   * @brief       返回命令的活动状态。
   *
   * @returns     如果启用则为 true；否则为 false。
   */
  virtual auto Active() -> bool = 0;

  /**
   * @brief       将命令附加到抽象按钮
   *
   * @details     绑定到按钮信号，然后从按钮发出适当的信号
   *
   * @param[in]   widget 抽象按钮子类化的小部件
   */
  virtual auto AttachToWidget(QAbstractButton* widget) -> void {
    connect(widget, &QAbstractButton::clicked, [this](bool) { this->Action()->trigger(); });

    connect(this->Action(), &QAction::changed, [this, widget] { widget->setEnabled(this->Active()); });
  }

  // 具有虚函数的类不应有公共的虚析构函数：
  ~ICommand() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommand, "sss.dscore.ICommand/1.0.0")
