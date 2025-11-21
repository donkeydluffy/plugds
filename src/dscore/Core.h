#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include <random>

#include "MainWindow.h"
#include "dscore/ICore.h"

namespace sss::dscore {
/**
 * @brief       The Core class is the root component for the application.
 *
 * @details     Provides an implementation of ICore which provides the main window for the application and
 *              provides the framework of the application (interfaces) which other components use to extend
 *              functionality.
 */
class Core : public sss::dscore::ICore {
 private:
  Q_OBJECT

  Q_INTERFACES(sss::dscore::ICore)

 public:
  /**
   * @brief       Constructs a new Core instance.
   */
  Core();

  /**
   * @brief       Destroys the Core.
   */
  ~Core() override;

  /**
   * @brief       Returns the main window instance.
   *
   * @details     Returns a pointer to the main window, this function always returns the same
   *              QMainWindow pointer so can be called by any part of the application to get a
   *              handle to the main window.
   *
   * @see         sss::dscore::ICore::mainWindow
   *
   * @returns     returns a pointer to the QMainWindow.
   */
  auto GetMainWindow() -> QMainWindow* override;

  /**
   * @brief       Opens the core.
   *
   * @details     Should be once by the application after the components are loaded.  Components connect
   *              to the sss::dscore::ICore::coreOpened signal to perform post load initialisation.
   *
   * @see         sss::dscore::ICore::open
   */
  auto Open() -> void;

  /**
   * @brief       Provides a random number between the minumum and maximum values.
   *
   * @param[in]   minimumValue the lower bound of the random number.
   * @param[in]   maximumValue the upper bound of the random number.
   *
   * @returns     a random number between the bounds.
   */

  auto Random(int minimum_value, int maximum_value) -> int override;

  /**
   * @brief       Returns the storage location.
   *
   * @note        This is the folder where persistent data should be stored, this is usually
   *              provided by the operating system, however, for a portable version of the
   *              application it may be another folder
   *
   * @returns     the folder where data should be saved.
   */
  auto StorageFolder() -> QString override;

  /**
   * @brief       Creates a new instance of the application context menu.
   *
   * @returns     the context menu.
   */
  auto ApplicationContextMenu() -> sss::dscore::IMenu* override;

 private:
  //! @cond

  QPointer<MainWindow> main_window_;

  std::random_device random_device_;

  std::mt19937* random_generator_;

  //! @endcond
};
}  // namespace sss::dscore
