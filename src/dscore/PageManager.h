#pragma once

#include <QHash>
#include <QObject>

#include "dscore/IPageManager.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace sss::dscore {

class IPageProvider;

class PageManager : public IPageManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IPageManager)
 public:
  explicit PageManager(QTabWidget* tab_widget);
  ~PageManager() override = default;

  void AddPage(IPageProvider* provider) override;
  void RemovePage(IPageProvider* provider) override;

 private:
  QTabWidget* tab_widget_ = nullptr;
  QHash<IPageProvider*, QWidget*> provider_to_widget_map_;
};

}  // namespace sss::dscore
