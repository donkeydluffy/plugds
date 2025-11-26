#include "PageManager.h"

#include <QTabWidget>
#include <QWidget>

#include "dscore/IContextManager.h"
#include "dscore/IPageProvider.h"

namespace sss::dscore {

PageManager::PageManager(QTabWidget* tab_widget) : tab_widget_(tab_widget) {
  if (tab_widget_ != nullptr) {
    connect(tab_widget_, &QTabWidget::currentChanged, this, &PageManager::onCurrentTabChanged);
  }
  context_manager_ = IContextManager::GetInstance();
}

void PageManager::AddPage(IPageProvider* provider) {
  if (context_manager_ == nullptr) {
    // TODO: 这里暂时先延时加载，以防止初始化顺序问题，后续可以考虑改进
    context_manager_ = IContextManager::GetInstance();
  }

  if ((provider == nullptr) || provider_to_widget_map_.contains(provider) || (context_manager_ == nullptr)) {
    return;
  }

  if ((tab_widget_ != nullptr) && !tab_widget_->isVisible()) {
    tab_widget_->setVisible(true);
  }

  auto* new_page = provider->CreatePage(tab_widget_);
  const auto title = provider->PageTitle();
  const auto context_id = provider->PageContextId();

  page_to_context_id_map_.insert(new_page, context_id);
  provider_to_widget_map_.insert(provider, new_page);

  const auto index = tab_widget_->addTab(new_page, title);
  tab_widget_->setCurrentIndex(index);
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

  page_to_context_id_map_.remove(widget_to_remove);
  provider_to_widget_map_.remove(provider);
  // The widget is deleted by Qt because its parent (the tab_widget_) has been removed.

  if ((tab_widget_ != nullptr) && tab_widget_->count() == 0) {
    tab_widget_->setVisible(false);
  }
}

void PageManager::onCurrentTabChanged(int index) {
  if (context_manager_ == nullptr) {
    return;
  }

  if (index == -1) {
    context_manager_->SetContext(kGlobalContext);
    return;
  }

  auto* current_widget = tab_widget_->widget(index);
  if (page_to_context_id_map_.contains(current_widget)) {
    const auto context_id = page_to_context_id_map_.value(current_widget);
    context_manager_->SetContext(context_id);
  }
}

}  // namespace sss::dscore
