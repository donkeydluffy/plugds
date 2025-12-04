#pragma once

#include <QSplashScreen>

namespace sss {
/**
 * @brief       应用程序的启动画面对话框。
 */
class SplashScreen : public QSplashScreen {
 private:
  Q_OBJECT

 private:
  /**
   * @brief       构造新的 SplashScreen 实例。
   *
   * @note        构造函数是私有的，因为 SplashScreen 是单例类，要获取 SplashScreen
   *              实例请使用 getInstance() 函数。
   */
  SplashScreen();

 public:
  /**
   * @brief       删除拷贝构造函数。
   */
  SplashScreen(const SplashScreen&) = delete;

  /**
   * @brief       删除赋值运算符。
   */
  SplashScreen& operator=(const SplashScreen&) = delete;
  /**
   * @brief       销毁 SplashScreen。
   */
  ~SplashScreen() override;

  /**
   * @brief           返回 SplashScreen 类的实例。
   *
   * @returns         SplashScreen 实例。
   */
  static auto GetInstance() -> SplashScreen*;

 protected:
  /**
   * @brief           绘制启动画面内容。
   *
   * @details         绘制启动画面，使用默认实现，然后在顶部叠加版本
   *                  信息。
   *
   * @param[in]       painter 用于绘制的画笔。
   */
  auto drawContents(QPainter* painter) -> void override;

 private:
  //! @cond

  float scale_factor_;

  //! @endcond
};
}  // namespace sss
