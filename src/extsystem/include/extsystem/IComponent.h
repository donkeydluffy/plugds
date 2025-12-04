#pragma once

#include <QObject>

#include "extsystem/ComponentSystemSpec.h"

#define SSSComponentInterfaceIID "sss.IComponent/1.0"

namespace sss::extsystem {
/**
 * @brief       IComponent 接口定义了可加载组件的契约。
 *
 * @details     组件必须实现的接口，插件系统将在适当的时候调用各种函数来
 *              加载和初始化插件。
 *
 * @class       sss::extsystem::IComponent IComponent.h <IComponent>
 */
class EXT_SYSTEM_DLLSPEC IComponent {
 public:
  /**
   * @brief       销毁 IComponent。
   */
  virtual ~IComponent();

  /**
   * @brief       初始化事件由组件加载器调用来初始化组件。
   *
   * @details     在所有组件加载完成后由组件加载器调用，按加载顺序调用。
   */
  virtual auto InitialiseEvent() -> void;

  /**
   * @brief       初始化完成事件函数在所有组件初始化完成后由组件加载器调用。
   *
   * @details     在所有组件初始化完成后由组件加载器调用，按加载顺序的逆序调用。
   */
  virtual auto InitialisationFinishedEvent() -> void;

  /**
   * @brief       结束事件方法在组件卸载前被调用。
   *
   * @note        对于所有已加载的组件，该事件按加载顺序的逆序调用，一旦每个组件
   *              都完成结束操作，组件管理器将以相同的顺序卸载所有组件。
   */
  virtual auto FinaliseEvent() -> void;
};
}  // namespace sss::extsystem

Q_DECLARE_INTERFACE(sss::extsystem::IComponent, SSSComponentInterfaceIID)
