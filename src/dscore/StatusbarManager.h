#pragma once

#include "dscore/IStatusbarManager.h"

namespace sss::dscore {
/**
 * @brief       The StatusbarManager class provides an IStatusbarManager implementation.
 */
class StatusbarManager : public IStatusbarManager {
 public:
  /**
   * @brief       Constructs a status bar manager.
   */
  StatusbarManager();
};
}  // namespace sss::dscore
