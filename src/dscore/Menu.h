#pragma once

#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QString>
#include <utility>

#include "dscore/IMenu.h"

namespace sss::dscore {
/**
 * @brief       The Menu class provides a QMenu based implementation of IMenu.
 *
 * @details     Represents a menu or menubar, allows commands to be registered in the menu and allows items
 *              to be logically grouped.
 */
class Menu : public sss::dscore::IMenu {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IMenu)

 private:
  /**
   * @brief       The GroupItem class defines an object that is used to store grouped menu items.
   */
  class GroupItem {
   public:
    /**
     * @brief       Constructs a group item.
     *
     * @details     Constructs a group item with the identifier.
     *
     * @param[in]   id is the identifier of the group.
     */
    explicit GroupItem(QString id) { id_ = std::move(id); }

    QString id_;  // NOLINT

    QList<QObject*> items_;  // NOLINT
  };

  /**
   * @brief       Constructs a Menu instance.
   */
  Menu();

  /**
   * @brief       Constructs a menu with the top level menu bar.
   *
   * @details     Constructs a menu with the top level menu bar.
   *
   * @see         sss::dscore::Menu::Menu
   *
   * @param[in]   menuBar is the menu bar that is to be attached to this instance.
   */
  explicit Menu(QMenuBar* menu_bar);

  /**
   * @brief       Constructs a menu for a main or sub menu.
   *
   * @details     Constructs a menu for a main or sub menu.
   *
   * @see         sss::dscore::Menu::Menu
   *
   * @param[in]   menu is the menu that is controlled by this instance.
   */
  explicit Menu(QMenu* menu);

 public:
  /**
   * @brief       Destroys the Menu.
   */
  ~Menu() override;

  /**
   * @brief       Returns the type of menu (Menu Bar and Menu)
   *
   * @see         sss::dscore::IMenu::type
   *
   * @returns     The sss::dscore::MenuTypes type of the menu.
   */
  auto Type() -> sss::dscore::MenuTypes override;

  /**
   * @brief       Returns the QMenu if this is a menu.
   *
   * @see         sss::dscore::IMenu::menu
   *
   * @returns     the menu instance.
   */
  auto GetMenu() -> QMenu* override;

  /**
   * @brief       Returns the QMenuBar if this is a menu bar.
   *
   * @see         sss::dscore::Menu::menuBar
   *
   * @returns     the menubar instance.
   */
  auto MenuBar() -> QMenuBar* override;

  /**
   * @brief       Inserts a new group into the start of the menu.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted at the start of the menu.
   *
   * @see         sss::dscore::Menu::insertGroup
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto InsertGroup(QString group_identifier) -> void override;

  /**
   * @brief       Appends a new group to the end of the menu.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be appended to the end of the menu.
   *
   * @see         sss::dscore::Menu::appendGroup
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto AppendGroup(QString group_identifier) -> void override;

  /**
   * @brief       Adds a new group before the given identifier.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted directly before the given existing identifier.
   *
   * @see         sss::dscore::Menu::addGroupBefore
   *
   * @param[in]   beforeIdentifier the identifier which this group is to be inserted before.
   * @param[in]   groupIdentifier the identifier of the group.
   *
   * @returns     true if added; otherwise false.
   */
  auto AddGroupBefore(QString before_identifier, QString group_identifier) -> bool override;

  /**
   * @brief       Adds a new group after the given identifier.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted directly after the given existing identifier.
   *
   * @see         sss::dscore::Menu::addGroupAfter
   *
   * @param[in]   afterIdentifier the existing identifier which the new group is to be inserted after.
   * @param[in]   groupIdentifier the identifier of the new group.
   *
   * @returns     true if added; otherwise false.
   */
  auto AddGroupAfter(QString after_identifier, QString group_identifier) -> bool override;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @details     The added command will be added to the end of the given group.
   *
   * @see         sss::dscore::Menu::appendCommand
   *
   * @param[in]   command the command to add.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void override;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @details     The added command will be added to the end of the given group.
   *
   * @see         sss::dscore::Menu::appendCommand
   *
   * @param[in]   commandIdentifier the identifier of the command.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto AppendCommand(QString command_identifier, QString group_identifier) -> void override;

  /**
   * @brief       Inserts a command to the start of a group.
   *
   * @details     The added command will be added to the start of the given group.
   *
   * @see         sss::dscore::Menu::insertCommand
   *
   * @param[in]   command the command to add.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto InsertCommand(sss::dscore::ICommand* command, QString group_identifier) -> void override;

  /**
   * @brief       Inserts a command to the start of a group.
   *
   * @details     The added command will be added to the start of the given group.
   *
   * @see         sss::dscore::Menu::insertCommand
   *
   * @param[in]   commandIdentifier the identifier of the command.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  auto InsertCommand(QString command_identifier, QString group_identifier) -> void override;

 private:
  /**
   * @brief       Finds the group by the identifier.
   *
   * @details     Finds the group by the identifier, if the group exists then the iterator will point at
   *              the item; otherwise it will point at the end of the list.
   *
   * @param[in]   groupIdentifier is the identifier of the group to find.
   *
   * @returns     the list iterator.
   */
  auto findGroup(const QString& group_identifier) -> QList<GroupItem>::const_iterator;

  /**
   * @brief       Gets a pointer to the action for to be inserted before.
   *
   * @details     Given a iterator into the group list, this function will find the action which is used as the root for
   * an append operation.
   *
   *              The group at the groupIterator is checked to see if it contains any items, if it does then the action
   * of the first item in the group is returned.
   *
   *              If the group does not contain any items, then the function will iterate through all the remaining
   * groups until a group is found that contains items and the action of the first item returned.
   *
   *              If no action could be found, then nullptr is returned.
   *
   * @param[in]   groupIterator is the iterator for the groups.
   *
   * @returns     a pointer to the QAction in the menu if found; otherwise nullptr.
   */
  auto getInsertAction(QList<sss::dscore::Menu::GroupItem>::const_iterator group_iterator) -> QAction*;

  /**
   * @brief       Gets a pointer to the action for to be appended after.
   *
   * @details     Given a iterator into the group list, this function will find the action which is used as the root for
   * an append operation.
   *
   *              The group at the groupIterator is checked to see if it contains any items, if it does then the action
   * of the first item in the group is returned.
   *
   *              If the group does not contain any items, then the function will iterate through all the remaining
   * groups until a group is found that contains items and the action of the first item returned.
   *
   *              If no action could be found, then nullptr is returned.
   *
   * @param[in]   groupIterator is the iterator for the groups.
   *
   * @returns     a pointer to the QAction in the menu if found; otherwise nullptr.
   */
  auto getAppendAction(QList<sss::dscore::Menu::GroupItem>::const_iterator group_iterator) -> QAction*;

  friend class CommandManager;

  //! @cond

  QMenuBar* menu_bar_;
  QMenu* menu_;
  QList<GroupItem> group_list_;

  //! @endcond
};
}  // namespace sss::dscore
