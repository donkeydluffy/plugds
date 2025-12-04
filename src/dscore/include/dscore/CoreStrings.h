#pragma once

#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief 核心共享字符串字典
 * 包含整个系统的标准术语翻译。
 * 使用静态方法提供标准翻译，防止重复翻译工作。
 */
class DS_CORE_DLLSPEC CoreStrings : public QObject {
  Q_OBJECT
 public:
  // --- General Operations ---
  static auto Ok() -> QString;
  static auto Cancel() -> QString;
  static auto Yes() -> QString;
  static auto No() -> QString;
  static auto Apply() -> QString;
  static auto Close() -> QString;
  static auto Save() -> QString;
  static auto Open() -> QString;
  static auto Delete() -> QString;

  // --- Status & Messages ---
  static auto Error() -> QString;
  static auto Warning() -> QString;
  static auto Information() -> QString;
  static auto Success() -> QString;
  static auto Loading() -> QString;

  // --- General UI Elements ---
  static auto Settings() -> QString;
  static auto Help() -> QString;
  static auto About() -> QString;
  static auto Language() -> QString;
  static auto Theme() -> QString;
};

}  // namespace sss::dscore
