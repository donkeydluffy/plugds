#pragma once

#include <QHash>
#include <QObject>

#include "dscore/IPageManager.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace sss::dscore {

class IContextManager;
class IPageProvider;

class PageManager : public IPageManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IPageManager)
 public:
  explicit PageManager(QTabWidget* tab_widget);
  ~PageManager() override = default;

  void AddPage(IPageProvider* provider) override;
  void RemovePage(IPageProvider* provider) override;

 private Q_SLOTS:
  void onCurrentTabChanged(int index);

 private:  // NOLINT
  QTabWidget* tab_widget_ = nullptr;
  IContextManager* context_manager_ = nullptr;
  QHash<IPageProvider*, QWidget*> provider_to_widget_map_;
  QHash<QWidget*, IPageProvider*> widget_to_provider_map_;
  QHash<QWidget*, int> page_to_context_id_map_;
};

}  // namespace sss::dscore
