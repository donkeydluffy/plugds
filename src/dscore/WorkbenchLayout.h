#pragma once

#include <QToolButton>
#include <QWidget>

#include "dscore/CoreSpec.h"    // For DS_CORE_DLLSPEC
#include "dscore/IWorkbench.h"  // For enums

QT_BEGIN_NAMESPACE
class QTabWidget;
class QSplitter;
QT_END_NAMESPACE

namespace sss::dscore {

class OverlayCanvas;

/**
 * @brief A reusable widget implementing the standard workspace layout:
 * - Left: TabWidget (Sidebar)
 * - Right: OverlayCanvas (3D View + HUDs)
 */
class DS_CORE_DLLSPEC WorkbenchLayout : public QWidget, public IWorkbench {
  Q_OBJECT
  Q_INTERFACES(sss::dscore::IWorkbench)

 public:
  explicit WorkbenchLayout(QWidget* parent = nullptr);
  ~WorkbenchLayout() override;

  // Proxy methods to components
  void AddSidePanel(const QString& id, QWidget* panel, const QString& title, const QIcon& icon = QIcon()) override;
  void SetBackgroundWidget(QWidget* widget) override;
  void AddSqueezeWidget(SqueezeSide side, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {}) override;
  void AddOverlayWidget(OverlayZone zone, QWidget* widget, int priority = 0, const QList<int>& visible_contexts = {},
                        const QList<int>& enable_contexts = {}) override;
  void ShowNotification(const QString& message, int duration_ms = 3000) override;

  /**
   * @brief Clears all content (Side panels, Squeeze, Overlays) from the workbench.
   */
  void Clear() override;

  [[nodiscard]] QSplitter* MainSplitter() const;

 private slots:
  void onToggleSidebar();

 private:  // NOLINT
  void setupUi();

  QSplitter* main_splitter_ = nullptr;
  QTabWidget* left_tab_widget_ = nullptr;
  OverlayCanvas* overlay_canvas_ = nullptr;
  QToolButton* sidebar_toggle_btn_ = nullptr;
  int last_sidebar_width_ = 250;  // Default width to restore
};

}  // namespace sss::dscore
