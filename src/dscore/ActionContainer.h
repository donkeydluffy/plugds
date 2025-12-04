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
 * @brief       ActionContainer 类提供了基于 QMenu/QToolBar 的 IActionContainer 实现。
 *
 * @details     表示菜单、菜单栏或工具栏。允许注册命令并进行逻辑分组。
 */
class ActionContainer : public sss::dscore::IActionContainer {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::IActionContainer)

 private:
  /**
   * @brief       GroupItem 类定义用于存储分组菜单项的对象。
   */
  class GroupItem {
   public:
    /**
     * @brief       构造组项。
     *
     * @details     使用标识符构造组项。
     *
     * @param[in]   id 组的标识符。
     * @param[in]   order 显示顺序。
     */
    explicit GroupItem(QString id, int order = 0) : id_(std::move(id)), order_(order) {}

    QString id_;  // NOLINT
    int order_;   // NOLINT

    QList<QObject*> items_;  // NOLINT
  };

  /**
   * @brief       构造 ActionContainer 实例。
   */
  ActionContainer();

  /**
   * @brief       为顶级菜单栏构造容器。
   *
   * @param[in]   menuBar 要附加到此实例的菜单栏。
   */
  explicit ActionContainer(QMenuBar* menu_bar);

  /**
   * @brief       为主菜单或子菜单构造容器。
   *
   * @param[in]   menu 由此实例控制的菜单。
   */
  explicit ActionContainer(QMenu* menu);

  /**
   * @brief       为工具栏构造容器。
   *
   * @param[in]   toolBar 由此实例控制的工具栏。
   */
  explicit ActionContainer(QToolBar* tool_bar);

 public:
  /**
   * @brief       销毁 ActionContainer。
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
