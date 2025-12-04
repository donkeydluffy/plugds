#include "extsystem/IComponentManager.h"

sss::extsystem::IComponentManager::~IComponentManager() = default;

auto sss::extsystem::IComponentManager::GetInstance() -> sss::extsystem::IComponentManager* {
  static IComponentManager component_manager;

  return &component_manager;
}

auto sss::extsystem::IComponentManager::AddObject(QObject* object) -> void {
  if (object == nullptr) {
    return;
  }
  object_list_.append(object);
}

auto sss::extsystem::IComponentManager::RemoveObject(QObject* object) -> void { object_list_.removeAll(object); }

auto sss::extsystem::IComponentManager::AllObjects() -> QList<QObject*> { return object_list_; }
