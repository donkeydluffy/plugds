#pragma once

#include <QIcon>
#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief The IMode interface defines a distinct application state or workspace.
 * Plugins implement this to define a major mode (e.g., "Scanning", "Reporting", "Settings").
 */
class DS_CORE_DLLSPEC IMode : public QObject {
  Q_OBJECT

 public:
  explicit IMode(QObject* parent = nullptr);
  ~IMode() override = default;

  /**
   * @brief Unique identifier for this mode.
   */
  [[nodiscard]] virtual QString Id() const = 0;

  /**
   * @brief Display name for the mode selector.
   */
  [[nodiscard]] virtual QString Title() const = 0;

  /**
   * @brief Icon for the mode selector.
   */
  [[nodiscard]] virtual QIcon Icon() const = 0;

  /**
   * @brief The context ID associated with this mode.
   * When this mode is active, the system ensures this Context ID is active.
   */
  [[nodiscard]] virtual int ContextId() const = 0;

  /**
   * @brief Priority for sorting modes in the mode selector (UI Shell).
   * Lower values = higher/earlier in list.
   */
  [[nodiscard]] virtual int Priority() const = 0;

  /**
   * @brief Called when the mode is activated.
   * The implementation should use the global IWorkbench to set up its view:
   * - SetBackgroundWidget(...)
   * - AddSidePanel(...)
   * - AddSqueezeWidget(...)
   */
  virtual void Activate() = 0;

  /**
   * @brief Called when the mode is deactivated.
   * The implementation should cleanup if necessary, though typically
   * the IWorkbench will handle context-based hiding automatically.
   * Explicit cleanup of widgets that shouldn't persist is recommended here.
   */
  virtual void Deactivate() = 0;
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::IMode, "sss.dscore.IMode")
