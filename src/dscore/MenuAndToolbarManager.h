#pragma once

#include <QObject>

namespace sss::dscore {

/**
 * @brief       通过协调提供者来管理菜单和工具栏的构建。
 *
 * @details     此类是 dscore 内部类，处理"阶段3"的UI组合。
 *              它发现所有 IMenuProvider 和 IToolbarProvider 实现，
 *              并调用它们来构建应用程序UI。
 */
class MenuAndToolbarManager : public QObject {
  Q_OBJECT

 public:
  explicit MenuAndToolbarManager(QObject* parent = nullptr);
  ~MenuAndToolbarManager() override = default;

  /**
   * @brief       收集贡献并构建UI。
   *              应在 CoreComponent::InitialisationFinishedEvent 期间调用一次。
   */
  void Build();
};

}  // namespace sss::dscore
