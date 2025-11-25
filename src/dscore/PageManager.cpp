#include "PageManager.h"

#include <QTabWidget>
#include <QWidget>

#include "dscore/IPageProvider.h"

namespace sss::dscore {

PageManager::PageManager(QTabWidget* tab_widget) : tab_widget_(tab_widget) {}

void PageManager::AddPage(IPageProvider* provider) {
  if ((provider == nullptr) || provider_to_widget_map_.contains(provider)) {
    return;
  }

  if ((tab_widget_ != nullptr) && !tab_widget_->isVisible()) {
    tab_widget_->setVisible(true);
  }

  auto* new_page = provider->CreatePage(tab_widget_);
  const auto title = provider->PageTitle();

  tab_widget_->addTab(new_page, title);
  provider_to_widget_map_.insert(provider, new_page);
}

void PageManager::RemovePage(IPageProvider* provider) {
  if ((provider == nullptr) || !provider_to_widget_map_.contains(provider)) {
    return;
  }

  auto* widget_to_remove = provider_to_widget_map_.value(provider);
  const int index = tab_widget_->indexOf(widget_to_remove);

  if (index != -1) {
    tab_widget_->removeTab(index);
  }

  // The widget is deleted by Qt because its parent (the tab_widget_) has been removed.
  provider_to_widget_map_.remove(provider);

  if ((tab_widget_ != nullptr) && tab_widget_->count() == 0) {
    tab_widget_->setVisible(false);
  }
}

}  // namespace sss::dscore
