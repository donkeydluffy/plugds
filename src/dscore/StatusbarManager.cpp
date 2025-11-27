#include "StatusbarManager.h"

namespace sss::dscore {

StatusbarManager::StatusbarManager(QStatusBar* status_bar) : status_bar_(status_bar) {}

void StatusbarManager::SetStatusMessage(const QString& message, int timeout) {
  if (status_bar_ != nullptr) {
    status_bar_->showMessage(message, timeout);
  }
}

void StatusbarManager::ClearStatusMessage() {
  if (status_bar_ != nullptr) {
    status_bar_->clearMessage();
  }
}

void StatusbarManager::AddPermanentWidget(QWidget* widget, int stretch) {
  if (status_bar_ != nullptr) {
    status_bar_->addPermanentWidget(widget, stretch);
  }
}

void StatusbarManager::RemovePermanentWidget(QWidget* widget) {
  if (status_bar_ != nullptr) {
    status_bar_->removeWidget(widget);
  }
}

}  // namespace sss::dscore
