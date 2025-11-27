#pragma once

#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

class IPageProvider;

/**
 * @brief The IPageManager class is a service interface for managing widget pages
 * in the main tabbed interface.
 */
class DS_CORE_DLLSPEC IPageManager : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief       Returns the IPageManager instance.
   */
  static auto GetInstance() -> IPageManager* { return sss::extsystem::GetTObject<IPageManager>(); }

  /**
   * @brief Virtual destructor.
   */
  ~IPageManager() override = default;

  /**
   * @brief Adds a page from a provider to the main tab widget.
   * @param provider The provider that supplies the widget and title.
   */
  virtual void AddPage(IPageProvider* provider) = 0;

  /**
   * @brief Removes a page associated with a provider from the main tab widget.
   * @param provider The provider whose page should be removed.
   */
  virtual void RemovePage(IPageProvider* provider) = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IPageManager, "sss.dscore.IPageManager/1.0.0")
