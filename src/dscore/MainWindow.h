#pragma once

#include <QAbstractButton>
#include <QAction>
#include <QList>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE

namespace sss::dscore {
QT_BEGIN_NAMESPACE
namespace Ui {  // NOLINT
class MainWindow;
}
QT_END_NAMESPACE

class IActionContainer;
class ICommand;
class ModeManager;
class WorkbenchLayout;

/**
 * @brief       MainWindow 类提供主应用程序窗口。
 * @class       sss::dscore::MainWindow MainWindow.h <MainWindow>
 */
class MainWindow : public QMainWindow {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       构造新的 MainWindow 实例，作为父对象的子对象。
   *
   * @param[in]   parent 所有者控件。
   */
  explicit MainWindow(QWidget* parent = nullptr);

  /**
   * @brief       销毁 MainWindow。
   */
  ~MainWindow() override;

  /**
   * @brief       初始化类为主应用程序设置各种子系统。
   */
  auto Initialise() -> void;

 protected:
  /**
   * @brief       Reimplements QMainWindow::closeEvent(QCloseEvent *event).
   */
  void closeEvent(QCloseEvent* close_event) override;

  /**
   * @brief       Reimplements QMainWindow::changeEvent(QEvent *event).
   */
  void changeEvent(QEvent* event) override;

 private:
  /**
   * @brief       在明暗模式切换时更新标题栏。
   */
  auto updateTitlebar() -> void;

  //! @cond

  Ui::MainWindow* ui_;

  bool application_hidden_;

  ModeManager* mode_manager_ = nullptr;
  WorkbenchLayout* workbench_layout_ = nullptr;

  //! @endcond
};
}  // namespace sss::dscore
