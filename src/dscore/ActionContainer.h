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
     * @param[in]   order is the display order.
     */
    explicit GroupItem(QString id, int order = 0) : id_(std::move(id)), order_(order) {}

    QString id_;  // NOLINT
    int order_;   // NOLINT

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

  auto GetWidget() -> QWidget* override;
  auto GetType() -> sss::dscore::ContainerType override;

  auto InsertGroup(QString group_identifier, int order) -> void override;

  auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void override;
  auto AppendCommand(QString command_identifier, QString group_identifier) -> void override;

  void SetOrder(int order) { order_ = order; }
  [[nodiscard]] int GetOrder() const { return order_; }

 private:
  auto findGroup(const QString& group_identifier) -> QList<GroupItem>::iterator;
  auto getInsertAction(QList<ActionContainer::GroupItem>::iterator group_iterator) -> QAction*;
  auto getAppendAction(QList<ActionContainer::GroupItem>::iterator group_iterator) -> QAction*;

  friend class CommandManager;

  //! @cond

  QMenuBar* menu_bar_ = nullptr;
  QMenu* menu_ = nullptr;
  QToolBar* tool_bar_ = nullptr;
  QList<GroupItem> group_list_;
  int order_ = 0;

  //! @endcond
};
}  // namespace sss::dscore
