#pragma once

#include <QSplashScreen>

namespace sss {
/**
 * @brief       The SplashScreen dialog for the application.
 */
class SplashScreen : public QSplashScreen {
 private:
  Q_OBJECT

 private:
  /**
   * @brief       Constructs a new SplashScreen instance.
   *
   * @note        The constructor is private as SplashScreen is a singleton class, to obtain the SplashScreen
   *              use the getInstance() function.
   */
  SplashScreen();

 public:
  /**
   * @brief       Delete the copy constructor.
   */
  SplashScreen(const SplashScreen&) = delete;

  /**
   * @brief       Delete the assignment operator.
   */
  SplashScreen& operator=(const SplashScreen&) = delete;
  /**
   * @brief       Destroys the SplashScreen.
   */
  ~SplashScreen() override;

  /**
   * @brief           Returns the instance of the SplashScreen class.
   *
   * @returns         the SplashScreen instance.
   */
  static auto GetInstance() -> SplashScreen*;

 protected:
  /**
   * @brief           Draw the contents of splash screen.
   *
   * @details         Draws the splash screen, uses the default implementation and then overlays version
   *                  information over the top.
   *
   * @param[in]       painter the painter to draw to.
   */
  auto drawContents(QPainter* painter) -> void override;

 private:
  //! @cond

  float scale_factor_;

  //! @endcond
};
}  // namespace sss
