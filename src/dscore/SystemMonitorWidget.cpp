#include "SystemMonitorWidget.h"

#include <QFile>
#include <QHBoxLayout>
#include <QProcess>
#include <QTextStream>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include <mach/mach.h>
#endif

namespace sss::dscore {
SystemMonitorWidget::SystemMonitorWidget(QWidget* parent) : QWidget(parent) {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(10, 0, 10, 0);
  layout->setSpacing(15);

  cpu_label_ = new QLabel("CPU: --.--%", this);
  mem_label_ = new QLabel("MEM: --.--%", this);

  // Set fixed width to prevent jitter when values change

  // Adjust width as needed for font size

  cpu_label_->setMinimumWidth(70);

  mem_label_->setMinimumWidth(70);

  // Simple style without padding to avoid clipping

  QString style = "QLabel { color: #888; font-size: 11px; }";

  cpu_label_->setStyleSheet(style);

  mem_label_->setStyleSheet(style);

  layout->addWidget(cpu_label_, 0, Qt::AlignVCenter);

  layout->addWidget(mem_label_, 0, Qt::AlignVCenter);

  // Ensure the widget is tall enough to prop up the status bar and contain the text

  setMinimumHeight(24);

  // Seed the previous CPU values immediately so the next update has a diff to work with.
  updateCpuUsage();

  update_timer_ = new QTimer(this);
  connect(update_timer_, &QTimer::timeout, this, &SystemMonitorWidget::updateStats);
  update_timer_->start(2000);  // Update every 2 seconds
}

SystemMonitorWidget::~SystemMonitorWidget() = default;

void SystemMonitorWidget::updateStats() {
  updateCpuUsage();
  updateMemoryUsage();
}

void SystemMonitorWidget::updateCpuUsage() {
  double cpu_percent = 0.0;
  bool valid = false;

#ifdef Q_OS_LINUX
  QFile file("/proc/stat");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    QString line = in.readLine();  // cpu ...
    if (line.startsWith("cpu ")) {
      // cpu  user nice system idle iowait irq softirq steal guest guest_nice
      QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      if (parts.size() >= 5) {
        uint64_t user = parts[1].toULongLong();
        uint64_t nice = parts[2].toULongLong();
        uint64_t system = parts[3].toULongLong();
        uint64_t idle = parts[4].toULongLong();

        // Generally total is sum of all fields, but user+nice+system+idle is the main chunk
        uint64_t total = user + nice + system + idle;

        if (parts.size() > 5) total += parts[5].toULongLong();  // iowait
        if (parts.size() > 6) total += parts[6].toULongLong();  // irq
        if (parts.size() > 7) total += parts[7].toULongLong();  // softirq

        uint64_t total_diff = total - prev_total_time_;
        uint64_t idle_diff = idle - prev_idle_time_;

        // Only update if we have a valid previous state (diff > 0) and it's not the very first run (where prev was 0)
        // If prev_total_time_ was 0, it means this is the first run, so we just store the values.
        if (prev_total_time_ > 0 && total_diff > 0) {
          cpu_percent = (1.0 - static_cast<double>(idle_diff) / total_diff) * 100.0;
          valid = true;
        }

        prev_total_time_ = total;
        prev_idle_time_ = idle;
      }
    }
    file.close();
  }
#elif defined(Q_OS_WIN)
  FILETIME idle_time, kernel_time, user_time;
  if (GetSystemTimes(&idle_time, &kernel_time, &user_time) != 0) {
    ULARGE_INTEGER ul_idle, ul_kernel, ul_user;
    ul_idle.LowPart = idle_time.dwLowDateTime;
    ul_idle.HighPart = idle_time.dwHighDateTime;
    ul_kernel.LowPart = kernel_time.dwLowDateTime;
    ul_kernel.HighPart = kernel_time.dwHighDateTime;
    ul_user.LowPart = user_time.dwLowDateTime;
    ul_user.HighPart = user_time.dwHighDateTime;

    uint64_t current_idle = ul_idle.QuadPart;
    // KernelTime includes IdleTime.
    uint64_t current_total = ul_kernel.QuadPart + ul_user.QuadPart;

    uint64_t total_diff = current_total - prev_total_time_;
    uint64_t idle_diff = current_idle - prev_idle_time_;

    if (prev_total_time_ > 0 && total_diff > 0) {
      cpu_percent = (1.0 - static_cast<double>(idle_diff) / total_diff) * 100.0;
      valid = true;
    }

    prev_total_time_ = current_total;
    prev_idle_time_ = current_idle;
  }
#endif

  if (valid) {
    cpu_label_->setText(QString("CPU: %1%").arg(cpu_percent, 5, 'f', 1));  // Fixed width format
  }
  // If not valid (first run), leave as default "--.--%" or previous value
}

void SystemMonitorWidget::updateMemoryUsage() {
  QString mem_text = "MEM: -%";
  QString tooltip = "";

#ifdef Q_OS_LINUX
  QFile file("/proc/meminfo");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    uint64_t mem_total = 0;
    uint64_t mem_available = 0;
    uint64_t swap_total = 0;
    uint64_t swap_free = 0;

    while (!in.atEnd()) {
      QString line = in.readLine();
      QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      if (parts.size() >= 2) {
        if (parts[0] == "MemTotal:")
          mem_total = parts[1].toULongLong();
        else if (parts[0] == "MemAvailable:")
          mem_available = parts[1].toULongLong();
        else if (parts[0] == "SwapTotal:")
          swap_total = parts[1].toULongLong();
        else if (parts[0] == "SwapFree:")
          swap_free = parts[1].toULongLong();
      }
    }
    file.close();

    if (mem_total > 0) {
      double mem_percent = (static_cast<double>(mem_total - mem_available) / mem_total) * 100.0;
      mem_text = QString("MEM: %1%").arg(mem_percent, 0, 'f', 1);

      uint64_t swap_used = swap_total - swap_free;

      tooltip = QString("RAM: %1 / %2 MB\nSwap: %3 / %4 MB")
                    .arg((mem_total - mem_available) / 1024)
                    .arg(mem_total / 1024)
                    .arg(swap_used / 1024)
                    .arg(swap_total / 1024);
    }
  }
#elif defined(Q_OS_WIN)
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex) != 0) {
    mem_text = QString("MEM: %1%").arg(statex.dwMemoryLoad);

    uint64_t total_commit = statex.ullTotalPageFile;
    uint64_t avail_commit = statex.ullAvailPageFile;
    uint64_t used_commit = total_commit - avail_commit;

    tooltip = QString("Physical Load: %1%\nVirtual Usage: %2 / %3 MB")
                  .arg(statex.dwMemoryLoad)
                  .arg(used_commit / (1024 * 1024))
                  .arg(total_commit / (1024 * 1024));
  }
#endif

  mem_label_->setText(mem_text);
  mem_label_->setToolTip(tooltip);
}

}  // namespace sss::dscore
