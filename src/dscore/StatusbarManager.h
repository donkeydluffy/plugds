#pragma once

#include <QObject>
#include <QStatusBar>

#include "dscore/IStatusbarManager.h"

namespace sss::dscore {

class StatusbarManager : public sss::dscore::IStatusbarManager {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IStatusbarManager)

 public:
  explicit StatusbarManager(QStatusBar* status_bar);
  ~StatusbarManager() override = default;

  void SetStatusMessage(const QString& message, int timeout) override;
  void ClearStatusMessage() override;
  void AddPermanentWidget(QWidget* widget, int stretch) override;
  void RemovePermanentWidget(QWidget* widget) override;

 private:
  QStatusBar* status_bar_ = nullptr;
};
}  // namespace sss::dscore
