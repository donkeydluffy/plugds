#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

class IMode;

/**
 * @brief Manages application modes and their activation.
 */
class DS_CORE_DLLSPEC IModeManager : public QObject {
  Q_OBJECT

 public:
  ~IModeManager() override = default;

  /**
   * @brief Registers a mode with the system.
   * @param mode The mode instance to register.
   */
  virtual void AddMode(IMode* mode) = 0;

  /**
   * @brief Activates a specific mode by ID.
   * @param id The unique identifier of the mode.
   */
  virtual void ActivateMode(const QString& id) = 0;

  /**
   * @brief Gets the currently active mode.
   * @return Pointer to the active mode, or nullptr if none.
   */
  [[nodiscard]] virtual IMode* ActiveMode() const = 0;

  /**
   * @brief Gets a list of all registered modes.
   */
  [[nodiscard]] virtual QList<IMode*> Modes() const = 0;

 signals:
  /**
   * @brief Signal emitted when a new mode is registered.
   */
  virtual void ModeAdded(IMode* mode) = 0;

  /**
   * @brief Signal emitted when the active mode changes.
   */
  virtual void ModeChanged(IMode* new_mode, IMode* old_mode) = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IModeManager, "sss.dscore.IModeManager")
