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
// class QTabWidget; // Removed
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

  /**
   * @brief       Creates a new instance of the application context menu.
   *
   * @returns     the context menu.
   */
  auto ApplicationContextMenu() -> sss::dscore::IActionContainer*;

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
   * @brief       Creates the placeholders for the default commands which other plugins
   *              can then use these in their own specific context.
   */
  auto createDefaultCommands() -> void;

  /**
   * @brief       Registers the default actions for the global context to the
   *              registered commands.
   */
  auto registerDefaultCommands() -> void;

  /**
   * @brief      Creates a Command and registers it with the system.
   */
  auto createCommand(QString command_id, QAbstractButton* button = nullptr,
                     QAction::MenuRole menu_role = QAction::NoRole) -> sss::dscore::ICommand*;

  /**
   * @brief       Adds a command to a menu.
   */
  auto addMenuCommand(const QString& command_id, const QString& menu_id, QString group_id = QString()) -> void;

  /**
   * @brief       Creates a menu with the given identifier.
   */
  auto createMenu(const QString& menu_id, const QString& parent_menu_id = QString(), int order = 0)
      -> sss::dscore::IActionContainer*;

  /**
   * @brief       Returns the IMenu pointer for a named menu.
   */
  auto findMenu(const QString& menu_id) -> sss::dscore::IActionContainer*;

  /**
   * @brief       Updates the title bar when light/dark mode changes.
   */
  auto updateTitlebar() -> void;

  /**
   * @brief       Updates command icons based on the current theme.
   */
  auto updateIcons(const QString& theme_id) -> void;

  /**
   * @brief       Sets up the toolbar size menu.
   */
  auto setupToolbarSizeMenu() -> void;

  //! @cond

  Ui::MainWindow* ui_;

  QAction* about_action_;
  QAction* quit_action_;
  QAction* preferences_action_;
  QAction* show_application_;
  QAction* hide_application_;

  bool application_hidden_;

  ModeManager* mode_manager_ = nullptr;
  WorkbenchLayout* workbench_layout_ = nullptr;

  //! @endcond
};
}  // namespace sss::dscore

#endif  // PINGNOO_COMPONENTS_CORE_MAINWINDOW_H
