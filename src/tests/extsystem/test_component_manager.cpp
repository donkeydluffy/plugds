#include <doctest/doctest.h>
#include <extsystem/IComponentManager.h>

#include <QCoreApplication>
#include <QObject>
#include <QPushButton>  // 用于测试不同的 QObject 类型
#include <QWidget>      // 用于测试不同的 QObject 类型

#include "test_classes.h"

// 测试夹具，确保 Qt 对象在测试中可以使用 QCoreApplication
struct QCoreApplicationFixture {
  QCoreApplication* app;  // NOLINT
  QCoreApplicationFixture() : app(nullptr) {
    if (QCoreApplication::instance() == nullptr) {
      static int argc = 0;
      app = new QCoreApplication(argc, nullptr);
    }
  }
  ~QCoreApplicationFixture() { delete app; }
};

namespace {
QCoreApplicationFixture app_fixture;
}

TEST_SUITE("IComponentManager") {
  TEST_CASE("GetInstance returns same instance (Singleton)") {
    sss::extsystem::IComponentManager* instance1 = sss::extsystem::IComponentManager::GetInstance();
    sss::extsystem::IComponentManager* instance2 = sss::extsystem::IComponentManager::GetInstance();
    CHECK(instance1 == instance2);
  }

  TEST_CASE("AddObject and AllObjects functionality") {
    sss::extsystem::IComponentManager* manager = sss::extsystem::IComponentManager::GetInstance();
    // 清除之前的状态以进行隔离测试
    QList<QObject*> current_objects = manager->AllObjects();
    for (QObject* obj : current_objects) {
      manager->RemoveObject(obj);
      delete obj;  // 清理管理的 QObject
    }
    CHECK(manager->AllObjects().isEmpty());

    auto* obj1 = new QObject(nullptr);
    auto* obj2 = new QWidget(nullptr);
    auto* obj3 = new QPushButton(nullptr);

    manager->AddObject(obj1);
    manager->AddObject(obj2);
    manager->AddObject(obj3);

    CHECK(manager->AllObjects().size() == 3);
    CHECK(manager->AllObjects().contains(obj1));
    CHECK(manager->AllObjects().contains(obj2));
    CHECK(manager->AllObjects().contains(obj3));

    // 测试添加 nullptr（不应崩溃或添加）
    manager->AddObject(nullptr);
    CHECK(manager->AllObjects().size() == 3);  // Still 3, nullptr should be ignored or handled by Qt's QList

    delete obj1;
    delete obj2;
    delete obj3;
  }

  TEST_CASE("RemoveObject functionality") {
    sss::extsystem::IComponentManager* manager = sss::extsystem::IComponentManager::GetInstance();
    // 清除之前的状态
    QList<QObject*> current_objects = manager->AllObjects();
    for (QObject* obj : current_objects) {
      manager->RemoveObject(obj);
      delete obj;
    }

    auto* obj1 = new QObject(nullptr);
    auto* obj2 = new QObject(nullptr);
    auto* obj3 = new QObject(nullptr);

    manager->AddObject(obj1);
    manager->AddObject(obj2);
    manager->AddObject(obj3);
    CHECK(manager->AllObjects().size() == 3);

    manager->RemoveObject(obj2);
    CHECK(manager->AllObjects().size() == 2);
    CHECK(manager->AllObjects().contains(obj1));
    CHECK_FALSE(manager->AllObjects().contains(obj2));
    CHECK(manager->AllObjects().contains(obj3));

    // 尝试移除一个不存在的对象
    auto* obj_non_existent = new QObject(nullptr);
    manager->RemoveObject(obj_non_existent);
    CHECK(manager->AllObjects().size() == 2);  // 大小应保持为 2

    // 移除剩余对象
    manager->RemoveObject(obj1);
    manager->RemoveObject(obj3);
    CHECK(manager->AllObjects().isEmpty());

    delete obj_non_existent;
  }

  TEST_CASE("GetTObject for specific type") {
    sss::extsystem::IComponentManager* manager = sss::extsystem::IComponentManager::GetInstance();
    // 清除之前的状态
    QList<QObject*> current_objects = manager->AllObjects();
    for (QObject* obj : current_objects) {
      manager->RemoveObject(obj);
      delete obj;
    }

    auto* base_obj = new QObject(nullptr);
    auto* widget_obj = new QWidget(nullptr);
    auto* button_obj = new QPushButton(nullptr);

    manager->AddObject(base_obj);
    manager->AddObject(widget_obj);
    manager->AddObject(button_obj);

    CHECK(sss::extsystem::GetTObject<QObject>() == base_obj);        // 第一个添加的 QObject
    CHECK(sss::extsystem::GetTObject<QWidget>() == widget_obj);      // 第一个添加的 QWidget
    CHECK(sss::extsystem::GetTObject<QPushButton>() == button_obj);  // 第一个添加的 QPushButton

    // 测试不存在的类型

    auto* custom_obj = sss::extsystem::GetTObject<MyCustomObject>();
    CHECK(custom_obj == nullptr);

    delete base_obj;
    delete widget_obj;
    delete button_obj;
  }

  TEST_CASE("GetTObjects for specific type") {
    sss::extsystem::IComponentManager* manager = sss::extsystem::IComponentManager::GetInstance();
    // 清除之前的状态
    QList<QObject*> current_objects = manager->AllObjects();
    for (QObject* obj : current_objects) {
      manager->RemoveObject(obj);
      delete obj;
    }

    auto* base_obj1 = new QObject(nullptr);
    auto* base_obj2 = new QObject(nullptr);
    auto* widget_obj1 = new QWidget(nullptr);
    auto* widget_obj2 = new QWidget(nullptr);

    manager->AddObject(base_obj1);
    manager->AddObject(widget_obj1);
    manager->AddObject(base_obj2);
    manager->AddObject(widget_obj2);

    QList<QObject*> all_qobjects = sss::extsystem::GetTObjects<QObject>();
    CHECK(all_qobjects.size() == 2);  // base_obj1 and base_obj2
    CHECK(all_qobjects.contains(base_obj1));
    CHECK(all_qobjects.contains(base_obj2));

    QList<QWidget*> all_qwidgets = sss::extsystem::GetTObjects<QWidget>();
    CHECK(all_qwidgets.size() == 2);  // widget_obj1 and widget_obj2
    CHECK(all_qwidgets.contains(widget_obj1));
    CHECK(all_qwidgets.contains(widget_obj2));

    // 测试不存在的类型

    QList<MyCustomObject*> custom_objects = sss::extsystem::GetTObjects<MyCustomObject>();
    CHECK(custom_objects.isEmpty());

    delete base_obj1;
    delete base_obj2;
    delete widget_obj1;
    delete widget_obj2;
  }
}
