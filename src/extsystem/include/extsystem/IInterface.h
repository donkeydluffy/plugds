#pragma once

#include <QObject>

#include "extsystem/ComponentSystemSpec.h"

namespace sss::extsystem {
/**
 * @brief       The IInterface class is the root level object for all objects.
 *
 * @details     All objects that reside in the component system should inherit this interface for future
 *              compatibility.
 *
 * @class       sss::extsystem::IInterface IInterface.h <IInterface>
 */
class EXT_SYSTEM_DLLSPEC IInterface : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief   Destroys the IInterface.
   *
   */
  ~IInterface() override = default;
};
}  // namespace sss::extsystem

Q_DECLARE_INTERFACE(sss::extsystem::IInterface, "com.sss.IInterface/1.0.0")
