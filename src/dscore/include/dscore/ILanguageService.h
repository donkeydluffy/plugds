#pragma once

#include <QLocale>
#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief 语言服务接口
 * 负责管理系统语言切换和加载组件翻译文件。
 */
class DS_CORE_DLLSPEC ILanguageService : public QObject {
  Q_OBJECT

 public:
  ~ILanguageService() override = default;

  /**
   * @brief 注册组件的翻译文件。
   * 组件在初始化期间调用此函数来告诉服务在哪里找到它们的 .qm 文件。
   *
   * @param component_name 组件的唯一标识符（例如："ws1", "dscore"）。
   * @param translation_path 资源路径（例如：":/ws1/i18n"）或文件系统路径。
   */
  virtual auto RegisterTranslator(const QString& component_name, const QString& translation_path) -> void = 0;

  /**
   * @brief 切换系统语言。
   * 为目标区域设置加载所有已注册的组件翻译文件并触发 QEvent::LanguageChange。
   *
   * @param locale 目标区域设置（例如：QLocale::Chinese）。
   */
  virtual auto SwitchLanguage(const QLocale& locale) -> void = 0;

  /**
   * @brief 获取当前活动的区域设置。
   * @returns 当前 QLocale。
   */
  [[nodiscard]] virtual auto GetCurrentLocale() const -> QLocale = 0;

 signals:
  /**
   * @brief 当语言发生变化时发出。
   * @param locale 新的区域设置。
   */
  void LanguageChanged(const QLocale& locale);
};

}  // namespace sss::dscore

Q_DECLARE_INTERFACE(sss::dscore::ILanguageService, "sss.dscore.ILanguageService/1.0.0")
