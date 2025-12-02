#pragma once

#include <QDebug>
#include <QMainWindow>
#include <QObject>

#include "dscore/CoreSpec.h"
#include "extsystem/IComponentManager.h"

namespace sss::dscore {

/**
 * @brief       The ICore interface is the root component for the application.
 *
 * @details     ICore should provide the main window for the application and the general framework of the
 *              application (interfaces) which other components use to extend  functionality.
 *
 * @class       sss::dscore::ICore ICore.h <ICore>
 */
class DS_CORE_DLLSPEC ICore : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       Returns the sss::dscore::ICore instance.
   */
  static auto GetInstance() -> ICore* { return sss::extsystem::GetTObject<ICore>(); }

  /**
   * @brief       Returns the main window instance.
   *
   * @details     Returns a pointer to the main window, this function always returns the same
   *              QMainWindow pointer so can be called by any part of the application to get a
   *              handle to the main window.
   *
   * @returns     returns a pointer to the QMainWindow.
   */
  virtual auto GetMainWindow() -> QMainWindow* = 0;

  /**
   * @brief       This signal is emitted after ICore has initialised.
   *
   * @details     The signal is emitted after all plugins have been loaded and initialised, allows
   *              components to do post-initialisation functions knowing that all plugins have
   *              initialised.
   */
  Q_SIGNAL void CoreOpened();

  /**
   * @brief       Provides a random number between the minumum and maximum values.
   *
   * @param[in]   minimumValue the lower bound of the random number.
   * @param[in]   maximumValue the upper bound of the random number.
   *
   * @returns     a random number between the bounds.
   */

  virtual auto Random(int minimum_value, int maximum_value) -> int = 0;

  /**
   * @brief       Returns the storage location.
   *
   * @note        This is the folder where persistent data should be stored, this is usually
   *              provided by the operating system, however, for a portable version of the
   *              application it may be another folder
   *
   * @returns     the folder where data should be saved.
   */
  virtual auto StorageFolder() -> QString = 0;

  /**
   * Classes with virtual functions should not have a public non-virtual destructor:
   */
  ~ICore() override = default;
};

/**
 * @brief       Convenience call to get the main window.
 *
 * @returns     The QMainWindow instance if created; otherwise nullptr;
 */
inline auto MainWindowInstance() -> QMainWindow* {
  auto* core = ICore::GetInstance();

  if (core != nullptr) return core->GetMainWindow();

  return nullptr;
}
}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ICore, "sss.dscore.ICore/1.0.0")
