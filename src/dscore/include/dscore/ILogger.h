#pragma once

// #include <spdlog/spdlog.h>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
/**
 * @brief       The ILogger class is used to allow spdlog to be managed in the main application.
 *
 * @class       sss::dscore::ILogger ILogger.h <ILogger>
 */
class NEDRYSOFT_CORE_DLLSPEC ILogger {
 public:
  ILogger() {

  };

  /**
   * @brief       Returns the spdlog logger.
   *
   * @returns     the logger
   */
  // virtual auto logger() -> std::shared_ptr<spdlog::logger> = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  virtual ~ILogger() = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ILogger, "com.nedrysoft.core.ILogger/1.0.0")
