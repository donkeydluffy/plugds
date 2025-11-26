#pragma once

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class ICommand;

enum class ActionContainerTypes : uint8_t { kIsMenuBar, kIsMenu, kIsSubMenu, kIsToolBar };

/**
 * @brief       The IActionContainer interface describes a container for QActions, like a menu, menubar, or
 * toolbar.
 *
 * @details     Represents a UI container for actions, allows commands to be registered and logically grouped.
 *
 * @class       sss::dscore::IActionContainer IActionContainer.h <IActionContainer>
 */
class DS_CORE_DLLSPEC IActionContainer : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the QMenu if this is a menu.
   *
   * @returns     the menu instance, or nullptr if not a menu.
   */
  virtual auto GetMenu() -> QMenu* = 0;

  /**
   * @brief       Returns the QMenuBar if this is a menu bar.
   *
   * @returns     the menubar instance, or nullptr if not a menu bar.
   */
  virtual auto MenuBar() -> QMenuBar* = 0;

  /**
   * @brief       Returns the QToolBar if this is a tool bar.
   *
   * @returns     the tool bar instance, or nullptr if not a tool bar.
   */
  virtual auto toolBar() -> QToolBar* = 0;

  /**
   * @brief       Inserts a new group into the start of the container.
   *
   * @details     Creates a named grouped section, allows commands to be grouped by function.
   *              The group will be inserted at the start.
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto InsertGroup(QString group_identifier) -> void = 0;

  /**
   * @brief       Appends a new group to the end of the container.
   *
   * @details     Creates a named grouped section, allows commands to be grouped by function.
   *              The group will be appended to the end.
   *
   * @param[in]   groupIdentifier the identifier of the group.
   */
  virtual auto AppendGroup(QString group_identifier) -> void = 0;

  /**
   * @brief       Adds a new group before the given identifier.
   *
   * @details     Creates a named grouped section, allows commands to be grouped by function.
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
   * @details     Creates a named grouped section, allows commands to be grouped by function.
   *              The group will be inserted directly after the given existing identifier.
   *
   * @param[in]   afterIdentifier the existing identifier which the new group is to be inserted after.
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
   * @brief       Returns the type of container (Menu Bar, Menu, ToolBar, etc.)
   *
   * @returns     The sss::dscore::ActionContainerTypes type of the container.
   */
  virtual auto Type() -> sss::dscore::ActionContainerTypes = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IActionContainer() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IActionContainer, "sss.dscore.IActionContainer/1.0.0")
