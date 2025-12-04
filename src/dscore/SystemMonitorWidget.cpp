#include "SystemMonitorWidget.h"

#include <QFile>
#include <QHBoxLayout>
#include <QProcess>
#include <QTextStream>

#include "dscore/CoreStrings.h"

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

  cpu_label_ = new QLabel(CoreStrings::CpuLabel(), this);
  mem_label_ = new QLabel(CoreStrings::MemLabel(), this);

  // 设置固定宽度以防止数值变化时抖动

  // 根据字体大小调整宽度

  cpu_label_->setMinimumWidth(70);

  mem_label_->setMinimumWidth(70);

  // 简单样式，无边距以避免裁剪

  QString style = "QLabel { color: #888; font-size: 11px; }";

  cpu_label_->setStyleSheet(style);

  mem_label_->setStyleSheet(style);

  layout->addWidget(cpu_label_, 0, Qt::AlignVCenter);

  layout->addWidget(mem_label_, 0, Qt::AlignVCenter);

  // 确保控件足够高以支撑状态栏并包含文本

  setMinimumHeight(24);

  // 立即初始化之前的CPU值，以便下次更新时有差值可以计算
  updateCpuUsage();

  update_timer_ = new QTimer(this);
  connect(update_timer_, &QTimer::timeout, this, &SystemMonitorWidget::updateStats);
  update_timer_->start(2000);  // 每2秒更新一次
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
      // CPU  用户态 优先级 系统态 空闲 等待IO 中断 软中断 虚拟化 虚拟化 nice
      QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      if (parts.size() >= 5) {
        uint64_t user = parts[1].toULongLong();
        uint64_t nice = parts[2].toULongLong();
        uint64_t system = parts[3].toULongLong();
        uint64_t idle = parts[4].toULongLong();

        // 通常总计是所有字段的总和，但用户态+优先级+系统态+空闲是主要部分
        uint64_t total = user + nice + system + idle;

        if (parts.size() > 5) total += parts[5].toULongLong();  // iowait
        if (parts.size() > 6) total += parts[6].toULongLong();  // irq
        if (parts.size() > 7) total += parts[7].toULongLong();  // softirq

        uint64_t total_diff = total - prev_total_time_;
        uint64_t idle_diff = idle - prev_idle_time_;

        // 只有在有有效的前一状态（差值 > 0）且不是首次运行时才更新（此时 prev 为 0）
        // 如果 prev_total_time_ 为 0，说明是首次运行，我们只需存储这些值
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
    // 内核时间包含空闲时间
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
    cpu_label_->setText(CoreStrings::CpuValue(cpu_percent));
  }
  // 如果无效（首次运行），保持默认的 "--.--%" 或之前的值
}

void SystemMonitorWidget::updateMemoryUsage() {
  QString mem_text = CoreStrings::MemValueInit();
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
      mem_text = CoreStrings::MemValue(mem_percent);

      uint64_t swap_used = swap_total - swap_free;

      tooltip = CoreStrings::MemTooltip((mem_total - mem_available) / 1024, mem_total / 1024, swap_used / 1024,
                                        swap_total / 1024);
    }
  }
#elif defined(Q_OS_WIN)
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex) != 0) {
    mem_text = CoreStrings::MemValue(static_cast<double>(statex.dwMemoryLoad));

    uint64_t total_commit = statex.ullTotalPageFile;
    uint64_t avail_commit = statex.ullAvailPageFile;
    uint64_t used_commit = total_commit - avail_commit;

    tooltip =
        CoreStrings::MemTooltipWin(statex.dwMemoryLoad, used_commit / (1024 * 1024), total_commit / (1024 * 1024));
  }
#endif

  mem_label_->setText(mem_text);
  mem_label_->setToolTip(tooltip);
}

}  // namespace sss::dscore
