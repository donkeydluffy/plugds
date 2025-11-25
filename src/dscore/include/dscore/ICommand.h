#pragma once

#include <QAbstractButton>
#include <QAction>
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
/**
 * @brief       ICommand interface
 *
 * @details     ICommand represents an actionable command in the system, commands
 *              are bound to QActions for given contexts, this allows the target of
 *              the command to change depending on the current context that the application
 *              is in.
 *
 * @class       sss::dscore::ICommand ICommand.h <ICommand>
 */
class DS_CORE_DLLSPEC ICommand : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the proxy action.
   *
   * @returns     the proxy action
   */
  virtual auto Action() -> QAction* = 0;

  /**
   * @brief       Sets the active state of the command.
   *
   * @param[in]   state true if active; otherwise false.
   */
  virtual auto SetActive(bool state) -> void = 0;

  /**
   * @brief       Returns the active state of the command.
   *
   * @returns     true if enabled; otherwise false.
   */
  virtual auto Active() -> bool = 0;

  /**
   * @brief       Attaches a command to an abstract push button
   *
   * @details     Binds to the buttons signals and then emits the appropriate
   *              signals from the push button
   *
   * @param[in]   widget the abstract button subclassed widget
   */
  virtual auto AttachToWidget(QAbstractButton* widget) -> void {
    connect(widget, &QAbstractButton::clicked, [this](bool) { this->Action()->trigger(); });

    connect(this->Action(), &QAction::changed, [this, widget] { widget->setEnabled(this->Active()); });
  }

  // Classes with virtual functions should not have a public non-virtual destructor:
  ~ICommand() override = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICommand, "sss.dscore.ICommand/1.0.0")
