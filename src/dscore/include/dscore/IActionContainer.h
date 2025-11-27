#pragma once

#include <QObject>
#include <QWidget>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class ICommand;

/**
 * @brief       Enumeration defining the type of the action container.
 */
enum class ContainerType : uint8_t { kMenuBar, kMenu, kToolBar, kRibbonBar, kUnknown };

/**
 * @brief       The IActionContainer interface describes a container for QActions.
 *
 * @details     Represents a UI container (Menu, Toolbar, etc.) for actions.
 *              Abstracts underlying Qt widgets to allow generic handling of menus and toolbars.
 *
 * @class       sss::dscore::IActionContainer IActionContainer.h <IActionContainer>
 */
class DS_CORE_DLLSPEC IActionContainer : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the underlying QWidget of this container.
   *
   * @returns     the widget instance, or nullptr if not applicable.
   */
  virtual auto GetWidget() -> QWidget* = 0;

  /**
   * @brief       Returns the type of container.
   *
   * @returns     The sss::dscore::ContainerType type of the container.
   */
  virtual auto GetType() -> sss::dscore::ContainerType = 0;

  /**
   * @brief       Inserts a new group into the container with a specific order.
   *
   * @details     Creates a named grouped section.
   *
   * @param[in]   group_identifier the identifier of the group.
   * @param[in]   order The display order/priority (lower is earlier/left). Default is 0.
   */
  virtual auto InsertGroup(QString group_identifier, int order) -> void = 0;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @param[in]   command the command to add.
   * @param[in]   group_identifier the identifier of the group.
   */
  virtual auto AppendCommand(sss::dscore::ICommand* command, QString group_identifier) -> void = 0;

  /**
   * @brief       Appends a command to the end of a group.
   *
   * @param[in]   command_identifier the identifier of the command.
   * @param[in]   group_identifier the identifier of the group.
   */
  virtual auto AppendCommand(QString command_identifier, QString group_identifier) -> void = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~IActionContainer() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IActionContainer, "sss.dscore.IActionContainer/1.0.0")
