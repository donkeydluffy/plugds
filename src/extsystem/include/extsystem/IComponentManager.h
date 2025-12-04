#pragma once

#include <QObject>

#include "extsystem/ComponentSystemSpec.h"

namespace sss::extsystem {
/**
 * @brief       IComponentManager 定义了用于管理已加载组件的类的契约。
 *
 * @details     除了处理组件的管理外，此类还为组件提供全局
 *              注册表。
 *
 * @class       sss::extsystem::IComponentManager IComponentManager.h <IComponentManager>
 */
class EXT_SYSTEM_DLLSPEC IComponentManager : public QObject {
 private:
  Q_OBJECT

 private:
  /**
   * @brief       构造一个新的 IComponentManager。
   */
  IComponentManager() = default;

  /**
   * @brief       销毁 IComponentManager。
   */
  ~IComponentManager() override;

 public:
  /**
   * @brief       向对象注册表添加一个对象。
   *
   * @param[in]   object 要存储的对象。
   */
  auto AddObject(QObject* object) -> void;

  /**
   * @brief       从对象注册表中移除一个对象。
   *
   * @param[in]   object 要移除的对象。
   */
  auto RemoveObject(QObject* object) -> void;

  /**
   * @brief       返回注册表中所有对象的列表。
   *
   * @returns     返回所有对象的列表。
   */
  auto AllObjects() -> QList<QObject*>;

  /**
   * @brief       返回 ComponentManager 对象的单例实例。
   *
   * @returns     单例实例
   */
  static auto GetInstance() -> IComponentManager*;

 private:
  QList<QObject*> object_list_;
};
}  // namespace sss::extsystem

/**
 * @brief       操作对象注册表的便捷函数。
 *
 * @code(.cpp)
 *              sss::extsystem::AddObject(object);
 *
 *              QList<QObject *> objectList = sss::extsystem::AllObjects();
 *
 *              auto object = sss::extsystem::GetObject<IInterface>();
 *
 *              QList<IInterface *> objectList = sss::extsystem::GetObjects<IInterface>();
 * @endcode
 */

namespace sss::extsystem {
/**
 * @brief       向注册表添加一个对象。
 *
 * @param[in]   object 要添加到注册表的对象。
 */
inline auto AddObject(QObject* object) -> void { IComponentManager::GetInstance()->AddObject(object); }

/**
 * @brief       从注册表中移除一个对象。
 *
 * @param[in]   object 要从注册表中移除的对象。
 */
inline auto RemoveObject(QObject* object) -> void { IComponentManager::GetInstance()->RemoveObject(object); }

/**
 * @brief       返回所有已注册的对象。
 *
 * @returns     对象列表。
 */
inline auto AllObjects() -> QList<QObject*> { return IComponentManager::GetInstance()->AllObjects(); }

/**
 * @brief       返回第一个匹配类型 T 的对象。
 *
 * @returns     类型 T 的对象。
 */
template <typename T>
inline auto GetTObject() -> T* {
  for (auto* object : IComponentManager::GetInstance()->AllObjects()) {
    auto cast_object = qobject_cast<T*>(object);

    if (cast_object) return cast_object;
  }

  return nullptr;
}

/**
 * @brief       返回所有实现类型 T 的对象。
 *
 * @returns     实现类型 T 的对象列表。
 */
template <typename T>
inline auto GetTObjects() -> QList<T*> {
  QList<T*> object_list;

  for (auto* object : IComponentManager::GetInstance()->AllObjects()) {
    auto cast_object = qobject_cast<T*>(object);

    if (cast_object) object_list.append(cast_object);
  }

  return object_list;
}
}  // namespace sss::extsystem
