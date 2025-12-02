#pragma once

#include <QObject>
#include <QString>

#include "dscore/CoreSpec.h"

namespace sss::dscore {

/**
 * @brief Core Shared Strings Dictionary
 * Contains standard terminology translations for the entire system.
 * Uses static methods to provide standard translations, preventing duplicate translation work.
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
