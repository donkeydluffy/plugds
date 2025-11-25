#pragma once

#include <QObject>
#include <QString>
#include <QWidget>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief The IPageProvider class is an interface that plugins can implement
 * to provide a widget page to be added to the main tabbed interface.
 */
class DS_CORE_DLLSPEC IPageProvider : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Virtual destructor.
   */
  ~IPageProvider() override = default;

  /**
   * @brief Creates the widget that will be shown in the tab page.
   * @param parent The parent for the new widget. Ownership is passed to the parent.
   * @return A pointer to the newly created QWidget.
   */
  virtual auto CreatePage(QWidget* parent) -> QWidget* = 0;

  /**
   * @brief Returns the title for the tab.
   * @return The string to be displayed as the tab's title.
   */
  [[nodiscard]] virtual auto PageTitle() const -> QString = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IPageProvider, "sss.dscore.IPageProvider/1.0.0")
