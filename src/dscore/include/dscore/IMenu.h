#pragma once

#include <QMenu>
#include <QMenuBar>

#include "dscore/CoreSpec.h"
#include "extsystem/IInterface.h"

namespace sss::dscore {

class ICommand;

enum class MenuTypes : uint8_t { kIsMenuBar, kIsMenu, kIsSubMenu };

/**
 * @brief       The IMenuInterface describes a menu or a menu bar.
 *
 * @details     Represents a menu or menubar, allows commands to be registered in the menu and allows items
 *              to be logically grouped.
 *
 * @class       sss::dscore::IMenu IMenu.h <IMenu>
 */
class NEDRYSOFT_CORE_DLLSPEC IMenu : public sss::extsystem::IInterface {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::extsystem::IInterface)

 public:
  /**
   * @brief       Returns the QMenu if this is a menu.
   *
   * @returns     the menu instance.
   */
  virtual auto GetMenu() -> QMenu* = 0;

  /**
   * @brief       Returns the QMenuBar if this is a menu bar.
   *
   * @returns     the menubar instance.
   */
  virtual auto MenuBar() -> QMenuBar* = 0;

  /**
   * @brief       Inserts a new group into the start of the menu.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted at the start of the menu.
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto InsertGroup(QString group_identifier) -> void = 0;

  /**
   * @brief       Appends a new group to the end of the menu.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be appended to the end of the menu.
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto AppendGroup(QString group_identifier) -> void = 0;

  /**
   * @brief       Adds a new group before the given identifier.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted directly before the given existing identifier.
   *
   * @param[in]   beforeIdentifier the identifier which this group is to be inserted before.
   * @param[in]   groupIdentifier the identifier of the group.
   *
   * @returns     true if added; otherwise false.
   */
  virtual auto AddGroupBefore(QString before_identifier, QString group_identifier) -> bool = 0;

  /**
   * @brief       Adds a new group after the given identifier.
   *
   * @details     Creates a named grouped section for a menu, allows commands to be grouped by function.
   *              The group will be inserted directly after the given existing identifier.
   *
   * @param[in]   afterIdentifier the identifier which this group is to be inserted after.
   * @param[in]   groupIdentifier the identifier of the group.
   *
   * @returns     true if added; otherwise false.
   */
  virtual auto AddGroupAfter(QString after_identifier, QString group_identifier) -> bool = 0;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @details     The added command will be added to the end of the given group.
   *
   * @param[in]   command the command to add.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void = 0;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @details     The added command will be added to the end of the given group.
   *
   * @param[in]   commandIdentifier the identifier of the command.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto AppendCommand(QString command_identifier, QString group_identifier) -> void = 0;

  /**
   * @brief       Inserts a command to the start of a group.
   *
   * @details     The added command will be added to the start of the given group.
   *
   * @param[in]   command the command to add.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto InsertCommand(sss::dscore::ICommand* command, QString group_identifier) -> void = 0;

  /**
   * @brief       Inserts a command to the start of a group.
   *
   * @details     The added command will be added to the start of the given group.
   *
   * @param[in]   commandIdentifier the identifier of the command.
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto InsertCommand(QString command_identifier, QString group_identifier) -> void = 0;

  /**
   * @brief       Returns the type of menu (Menu Bar and Menu)
   *
   * @returns     The sss::dscore::MenuTypes type of the menu.
   */
  virtual auto Type() -> sss::dscore::MenuTypes = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IMenu() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMenu, "com.nedrysoft.core.IMenu/1.0.0")
