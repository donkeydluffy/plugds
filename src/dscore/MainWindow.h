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
class ModeManager;      // Forward declaration
class WorkbenchLayout;  // Forward declaration

/**
 * @brief       The MainWindow class provides the Main Application window.
 */
class MainWindow : public QMainWindow {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Constructs a new MainWindow instance which is a child of the parent.
   *
   * @param[in]   parent the owner widget.
   */
  explicit MainWindow(QWidget* parent = nullptr);

  /**
   * @brief       Destroys the MainWindow.
   */
  ~MainWindow() override;

  /**
   * @brief       The initialise class sets up various subsystems for the main application.
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
   * @brief       Updates the title bar when light/dark mode changes.
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
