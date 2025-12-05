#pragma once

#include <QJsonObject>
#include <QObject>

#include "dscore/CoreSpec.h"

namespace sss::dscore {
/**
 * @brief       IConfiguration 接口为数据加载和保存提供契约。
 *
 * @class       sss::dscore::IConfiguration IConfiguration.h <IConfiguration>
 */
class DS_CORE_DLLSPEC IConfiguration {
 public:
  /**
   * @brief       将配置保存到 JSON 对象。
   *
   * @returns     JSON 配置对象。
   */
  virtual auto SaveConfiguration() -> QJsonObject = 0;

  /**
   * @brief       加载配置。
   *
   * @param[in]   configuration JSON 格式的配置对象。
   *
   * @returns     如果加载成功返回 true；否则返回 false。
   */
  virtual auto LoadConfiguration(QJsonObject configuration) -> bool = 0;

  virtual ~IConfiguration() = default;
};
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IConfiguration, "sss.dscore.IConfiguration/1.0.0")
