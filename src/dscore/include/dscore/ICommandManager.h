#pragma once

#include <QAction>
#include <QList>
#include <QObject>
#include <utility>

#include "dscore/IActionContainer.h"
#include "dscore/ICommand.h"
#include "dscore/IContextManager.h"

namespace sss::dscore {
/**
 * @brief       ICommandManager 接口负责创建命令并在应用程序上下文更改时更新它们。
 *
 * @details     它提供了创建菜单和定位命令的方法。
 *
 * @class       sss::dscore::ICommandManager ICommandManager.h <ICommandManager>
 */
class DS_CORE_DLLSPEC ICommandManager : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       获取 sss::dscore::ICommandManager 实例。
   *
   * @returns     ICommandManager 实例。
   */
  static auto GetInstance() -> ICommandManager* { return sss::extsystem::GetTObject<ICommandManager>(); }

  /**
   * @brief       为给定上下文向命令注册 QAction。
   *
   * @details     此函数通过命令ID注册动作，如果命令已存在，
   *              则为给定上下文添加动作，否则创建新命令。
   *
   * @see         sss::dscore::ICommandManager::registerAction
   *
   * @param[in]   action 动作。
   * @param[in]   id 命令的标识符。
   * @param[in]   contexts 此动作有效的上下文。
   *
   * @returns     指向 ICommand 的指针
   */
  virtual auto RegisterAction(QAction* action, QString id, const sss::dscore::ContextList& visibility_contexts,
                              const sss::dscore::ContextList& enabled_contexts) -> sss::dscore::ICommand* = 0;

  virtual auto RegisterAction(QAction* action, QString id, int context_id) -> sss::dscore::ICommand* {
    sss::dscore::ContextList single_context_list;
    single_context_list << context_id;
    return RegisterAction(action, std::move(id), single_context_list, single_context_list);
  }

  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command,
                              const sss::dscore::ContextList& visibility_contexts,
                              const sss::dscore::ContextList& enabled_contexts) -> bool = 0;

  virtual auto RegisterAction(QAction* action, sss::dscore::ICommand* command, int context_id) -> bool {
    sss::dscore::ContextList single_context_list;
    single_context_list << context_id;
    return RegisterAction(action, command, single_context_list, single_context_list);
  }

  /**
   * @brief       设置当前活动上下文。
   *
   * @details     更新所有已注册的命令，使它们连接到该上下文的正确 QAction。
   *
   * @param[in]   context_id 要设置为活动的上下文。
   */
  virtual auto SetContext(int context_id) -> void = 0;

  /**
   * @brief       创建通用动作容器。
   *
   * @param[in]   identifier 容器的唯一标识符。
   * @param[in]   type 容器的类型。
   * @param[in]   parent_container 父容器（可选）。
   * @param[in]   order 显示优先级（越小越早）。默认为 0。
   *
   * @returns     新的 IActionContainer 实例。
   */
  virtual auto CreateActionContainer(const QString& identifier, sss::dscore::ContainerType type,
                                     IActionContainer* parent_container, int order)
      -> sss::dscore::IActionContainer* = 0;

  /**
   * @brief       查找容器。
   *
   * @details     通过给定标识符查找容器。
   *
   * @param[in]   identifier 容器的唯一标识符。
   *
   * @returns     如果存在则返回 IActionContainer 实例；否则返回 nullptr。
   */
  virtual auto FindContainer(const QString& identifier) -> sss::dscore::IActionContainer* = 0;

  /**
   * @brief       查找命令。
   *
   * @details     通过给定标识符查找已注册的命令。
   *
   * @param[in]   identifier 命令的标识符。
   *
   * @returns     如果命令存在则返回 ICommand 实例；否则返回 nullptr；
   */
  virtual auto FindCommand(const QString& identifier) -> sss::dscore::ICommand* = 0;

  /**
   * @brief       重新翻译由此管理器管理的UI元素（菜单、命令）。
   *              当应用程序语言更改时应调用。
   */
  virtual auto RetranslateUi() -> void = 0;

  // 具有虚函数的类不应有公共的虚析构函数：
  ~ICommandManager() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommandManager, "sss.dscore.ICommandManager/1.0.0")
