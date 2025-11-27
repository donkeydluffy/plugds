#include "PageManager.h"

#include <spdlog/spdlog.h>

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
  SPDLOG_INFO("PageManager::AddPage called with provider: {}", provider != nullptr ? "valid" : "null");

  if (context_manager_ == nullptr) {
    context_manager_ = IContextManager::GetInstance();
  }

  if ((provider == nullptr) || provider_to_widget_map_.contains(provider) || (context_manager_ == nullptr)) {
    SPDLOG_WARN("PageManager::AddPage - early return: provider={}, contains={}, context_manager={}",
                provider != nullptr, provider_to_widget_map_.contains(provider), context_manager_ != nullptr);
    return;
  }

  if ((tab_widget_ != nullptr) && !tab_widget_->isVisible()) {
    tab_widget_->setVisible(true);
    SPDLOG_INFO("PageManager::AddPage - made tab widget visible");
  }

  auto* new_page = provider->CreatePage(tab_widget_);
  const auto title = provider->PageTitle();
  const auto context_id = provider->PageContextId();
  const int new_order = provider->PageOrder();

  SPDLOG_INFO("PageManager::AddPage - creating page: title='{}', context_id={}, order={}", title.toStdString(),
              context_id, new_order);

  page_to_context_id_map_.insert(new_page, context_id);
  provider_to_widget_map_.insert(provider, new_page);
  widget_to_provider_map_.insert(new_page, provider);

  // Find insertion index based on order
  int insert_index = tab_widget_->count();  // Default: append
  for (int i = 0; i < tab_widget_->count(); ++i) {
    QWidget* existing_widget = tab_widget_->widget(i);
    if (widget_to_provider_map_.contains(existing_widget)) {
      IPageProvider* existing_provider = widget_to_provider_map_.value(existing_widget);
      if (existing_provider->PageOrder() > new_order) {
        insert_index = i;
        break;
      }
    }
  }

  const auto index = tab_widget_->insertTab(insert_index, new_page, title);
  tab_widget_->setCurrentIndex(index);

  SPDLOG_INFO("PageManager::AddPage - inserted tab at index {}, total tabs: {}", index, tab_widget_->count());
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
  widget_to_provider_map_.remove(widget_to_remove);
  // The widget is deleted by Qt because its parent (the tab_widget_) has been removed.

  if ((tab_widget_ != nullptr) && tab_widget_->count() == 0) {
    tab_widget_->setVisible(false);
  }
}

void PageManager::onCurrentTabChanged(int index) {
  SPDLOG_INFO("PageManager::onCurrentTabChanged called with index: {}", index);

  if (context_manager_ == nullptr) {
    SPDLOG_WARN("PageManager::onCurrentTabChanged - context_manager_ is null");
    return;
  }

  if (index == -1) {
    SPDLOG_INFO("PageManager::onCurrentTabChanged - setting global context");
    context_manager_->SetContext(kGlobalContext);
    return;
  }

  auto* current_widget = tab_widget_->widget(index);
  if (page_to_context_id_map_.contains(current_widget)) {
    const auto context_id = page_to_context_id_map_.value(current_widget);
    SPDLOG_INFO("PageManager::onCurrentTabChanged - setting context to: {}", context_id);
    context_manager_->SetContext(context_id);
  } else {
    SPDLOG_WARN("PageManager::onCurrentTabChanged - widget not found in page_to_context_id_map_");
  }
}

}  // namespace sss::dscore
