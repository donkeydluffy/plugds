/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 27/03/2020.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PINGNOO_COMPONENTS_CORE_MAINWINDOW_H
#define PINGNOO_COMPONENTS_CORE_MAINWINDOW_H

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

#endif  // PINGNOO_COMPONENTS_CORE_MAINWINDOW_H
