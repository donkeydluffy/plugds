#pragma once

#include <QJsonObject>
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
/**
 * @brief       The IConfiguration interface provides a contract for loading & saving data.
 *
 * @class       sss::dscore::IConfiguration IConfiguration.h <IConfiguration>
 */
class DS_CORE_DLLSPEC IConfiguration {
 public:
  /**
   * @brief       Saves the configuration to a JSON object.
   *
   * @returns     the JSON configuration.
   */
  virtual auto SaveConfiguration() -> QJsonObject = 0;

  /**
   * @brief       Loads the configuration.
   *
   * @param[in]   configuration the configuration as JSON object.
   *
   * @returns     true if loaded; otherwise false.
   */
  virtual auto LoadConfiguration(QJsonObject configuration) -> bool = 0;

  // Classes with virtual functions should not have a public non-virtual destructor:
  virtual ~IConfiguration() = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IConfiguration, "sss.dscore.IConfiguration/1.0.0")
