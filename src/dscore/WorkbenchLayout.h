#pragma once

#include <QToolButton>
#include <QWidget>
#include <functional>

#include "dscore/CoreSpec.h"
#include "dscore/IWorkbench.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
class QSplitter;
QT_END_NAMESPACE

namespace sss::dscore {

class OverlayCanvas;
class ModeSwitcher;

/**
 * @brief 实现标准工作空间布局的可重用控件：
 * - 左侧：TabWidget（侧边栏）
 * - 右侧：OverlayCanvas（3D视图+HUD）
 */
class DS_CORE_DLLSPEC WorkbenchLayout : public QWidget, public IWorkbench {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IWorkbench)

 public:
  explicit WorkbenchLayout(QWidget* parent = nullptr);
  ~WorkbenchLayout() override;

  // 到组件的代理方法
  void AddSidePanel(const QString& id, QWidget* panel, const QString& title, const QIcon& icon) override;
  void SetBackgroundWidget(QWidget* widget) override;
  void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority, const QList<int>& visible_contexts,
                        const QList<int>& enable_contexts) override;
  void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority, const QList<int>& visible_contexts,
                        const QList<int>& enable_contexts) override;
  void ShowNotification(const QString& message, int duration_ms) override;

  /**
   * @brief 从工作台中清除所有内容（侧边面板、挤压控件、覆盖控件）。
   */
  void Clear() override;

  void AddModeButton(const QString& id, const QString& title, const QIcon& icon) override;
  void SetActiveModeButton(const QString& id) override;
  void SetModeSwitchCallback(std::function<void(const QString&)> callback) override;

  [[nodiscard]] QSplitter* MainSplitter() const;

 private slots:
  void onToggleSidebar();

 private:  // NOLINT
  void setupUi();

  QSplitter* main_splitter_ = nullptr;
  QTabWidget* left_tab_widget_ = nullptr;
  OverlayCanvas* overlay_canvas_ = nullptr;
  QToolButton* sidebar_toggle_btn_ = nullptr;
  ModeSwitcher* mode_switcher_ = nullptr;
  std::function<void(const QString&)> mode_switch_callback_;
  int last_sidebar_width_ = 250;  // 要恢复的默认宽度
};

}  // namespace sss::dscore
