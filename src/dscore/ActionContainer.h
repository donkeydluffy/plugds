#pragma once

#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QString>
#include <QToolBar>
#include <utility>

#include "dscore/IActionContainer.h"

namespace sss::dscore {
/**
 * @brief       The ActionContainer class provides a QMenu/QToolBar based implementation of IActionContainer.
 *
 * @details     Represents a menu, menubar, or toolbar. Allows commands to be registered and logically grouped.
 */
class ActionContainer : public sss::dscore::IActionContainer {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IActionContainer)

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
   * @brief       Constructs an ActionContainer instance.
   */
  ActionContainer();

  /**
   * @brief       Constructs a container for a top level menu bar.
   *
   * @param[in]   menuBar is the menu bar that is to be attached to this instance.
   */
  explicit ActionContainer(QMenuBar* menu_bar);

  /**
   * @brief       Constructs a container for a main or sub menu.
   *
   * @param[in]   menu is the menu that is controlled by this instance.
   */
  explicit ActionContainer(QMenu* menu);

  /**
   * @brief       Constructs a container for a tool bar.
   *
   * @param[in]   toolBar is the tool bar that is controlled by this instance.
   */
  explicit ActionContainer(QToolBar* tool_bar);

 public:
  /**
   * @brief       Destroys the ActionContainer.
   */
  ~ActionContainer() override;

  auto Type() -> sss::dscore::ActionContainerTypes override;

  auto GetMenu() -> QMenu* override;
  auto MenuBar() -> QMenuBar* override;
  auto ToolBar() -> QToolBar* override;

  auto InsertGroup(QString group_identifier) -> void override;
  auto AppendGroup(QString group_identifier) -> void override;
  auto AddGroupBefore(QString before_identifier, QString group_identifier) -> bool override;
  auto AddGroupAfter(QString after_identifier, QString group_identifier) -> bool override;

  auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void override;
  auto AppendCommand(QString command_identifier, QString group_identifier) -> void override;

  auto InsertCommand(sss::dscore::ICommand* command, QString group_identifier) -> void override;
  auto InsertCommand(QString command_identifier, QString group_identifier) -> void override;

 private:
  auto findGroup(const QString& group_identifier) -> QList<GroupItem>::const_iterator;
  auto getInsertAction(QList<ActionContainer::GroupItem>::const_iterator group_iterator) -> QAction*;
  auto getAppendAction(QList<ActionContainer::GroupItem>::const_iterator group_iterator) -> QAction*;

  friend class CommandManager;

  //! @cond

  QMenuBar* menu_bar_ = nullptr;
  QMenu* menu_ = nullptr;
  QToolBar* tool_bar_ = nullptr;
  QList<GroupItem> group_list_;

  //! @endcond
};
}  // namespace sss::dscore
