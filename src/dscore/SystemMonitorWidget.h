#pragma once

#include <QLabel>
#include <QTimer>
#include <QWidget>

namespace sss::dscore {
class SystemMonitorWidget : public QWidget {
  Q_OBJECT
 public:
  explicit SystemMonitorWidget(QWidget* parent = nullptr);
  ~SystemMonitorWidget() override;

 private slots:
  void updateStats();

 private:  // NOLINT
  void updateCpuUsage();
  void updateMemoryUsage();

  QLabel* cpu_label_ = nullptr;
  QLabel* mem_label_ = nullptr;
  QTimer* update_timer_ = nullptr;

  // CPU calculation state
  uint64_t prev_idle_time_ = 0;
  uint64_t prev_total_time_ = 0;
};
}  // namespace sss::dscore
