#pragma once

#include <QObject>

// 为测试中的 Q_OBJECT 宏兼容性定义 MyCustomObject
class MyCustomObject : public QObject {
  Q_OBJECT
 public:
  explicit MyCustomObject(QObject* parent = nullptr) : QObject(parent) {}
};
