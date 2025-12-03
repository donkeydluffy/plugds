#pragma once

#include <QObject>

namespace sss::dscore {

/**
 * @brief       Manages the construction of menus and toolbars by orchestrating providers.
 *
 * @details     This class is internal to dscore and handles the "Phase 3" UI composition.
 *              It discovers all IMenuProvider and IToolbarProvider implementations and
 *              invokes them to build the application UI.
 */
class MenuAndToolbarManager : public QObject {
  Q_OBJECT

 public:
  explicit MenuAndToolbarManager(QObject* parent = nullptr);
  ~MenuAndToolbarManager() override = default;

  /**
   * @brief       Collects contributions and builds the UI.
   *              Should be called once during CoreComponent::InitialisationFinishedEvent.
   */
  void Build();
};

}  // namespace sss::dscore
