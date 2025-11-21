#pragma once

#include <QObject>

#include "extsystem/ComponentSystemSpec.h"

namespace sss::extsystem {
/**
 * @brief       The IComponentManager defines the contract for a class to manage loaded components.
 *
 * @details     In addition to handling the management of components, this class also provides a global
 *              registry for components.
 *
 * @class       sss::extsystem::IComponentManager IComponentManager.h <IComponentManager>
 */
class COMPONENT_SYSTEM_DLLSPEC IComponentManager : public QObject {
 private:
  Q_OBJECT

 private:
  /**
   * @brief       `Constructs a new IComponentManager.
   */
  IComponentManager() = default;

  /**
   * @brief       Destroys the IComponentManager.
   */
  ~IComponentManager() override;

 public:
  /**
   * @brief       Add an object to the object registry.
   *
   * @param[in]   object object to store.
   */
  auto AddObject(QObject* object) -> void;

  /**
   * @brief       Removes an object to the object registry.
   *
   * @param[in]   object object to remove.
   */
  auto RemoveObject(QObject* object) -> void;

  /**
   * @brief       Returns a list of all objects in the registry.
   *
   * @returns     returns a list of all objects.
   */
  auto AllObjects() -> QList<QObject*>;

  /**
   * @brief       Returns the singleton instance to the ComponentManager object.
   *
   * @returns     the singleton instance
   */
  static auto GetInstance() -> IComponentManager*;

 private:
  QList<QObject*> object_list_;
};
}  // namespace sss::extsystem

/**
 * @brief       Convenience functions to manipulate the object registry.
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
 * @brief       Adds an object to the registry.
 *
 * @param[in]   object the object to add to the registry.
 */
inline auto AddObject(QObject* object) -> void { IComponentManager::GetInstance()->AddObject(object); }

/**
 * @brief       Removes an object to the registry.
 *
 * @param[in]   object the object to remove from the registry.
 */
inline auto RemoveObject(QObject* object) -> void { IComponentManager::GetInstance()->RemoveObject(object); }

/**
 * @brief       Returns all registered objects.
 *
 * @returns     the list of objects.
 */
inline auto AllObjects() -> QList<QObject*> { return IComponentManager::GetInstance()->AllObjects(); }

/**
 * @brief       Returns the first matching object of type T.
 *
 * @returns     the object of type T.
 */
template <typename T>
inline auto GetObject() -> T* {
  for (auto* object : IComponentManager::GetInstance()->AllObjects()) {
    auto cast_object = qobject_cast<T*>(object);

    if (cast_object) return cast_object;
  }

  return nullptr;
}

/**
 * @brief       Returns all objects that implement type T.
 *
 * @returns     the list of objects implementing type T.
 */
template <typename T>
inline auto GetObjects() -> QList<T*> {
  QList<T*> object_list;

  for (auto* object : IComponentManager::GetInstance()->AllObjects()) {
    auto cast_object = qobject_cast<T*>(object);

    if (cast_object) object_list.append(cast_object);
  }

  return object_list;
}
}  // namespace sss::extsystem
