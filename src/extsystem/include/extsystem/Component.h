#pragma once

#include <QDataStream>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QVersionNumber>

#include "extsystem/ComponentLoader.h"
#include "extsystem/ComponentSystemSpec.h"

namespace sss::extsystem {
/**
 * @brief       Component 类保存已发现组件的信息。
 *
 * @class       sss::extsystem::Component Component.h <Component>
 */
class EXT_SYSTEM_DLLSPEC Component {
 public:
  /**
   * @brief       构造新的 Component。
   */
  Component();

  /**
   * @brief       使用给定信息构造新的 Component。
   *
   * @param[in]   name 组件的名称。
   * @param[in]   filename 组件的文件名。
   * @param[in]   metadata 从组件文件检索的元数据。
   */
  Component(const QString& name, const QString& filename, const QJsonObject& metadata);

  /**
   * @brief       向此组件添加组件依赖项。
   *
   * @param[in]   dependency      所需的依赖项
   * @param[in]   version_number   所需的依赖项版本
   */
  auto AddDependency(Component* dependency, QVersionNumber version_number) -> void;

  /**
   * @brief       返回组件的名称。
   *
   * @returns     组件名称。
   */
  auto Name() -> QString;

  /**
   * @brief       返回组件的文件名。
   *
   * @returns     组件文件名。
   *
   */
  auto Filename() -> QString;

  /**
   * @brief       返回组件的解码元数据作为 JSON 对象。
   *
   * @returns     组件元数据。
   */
  auto Metadata() -> QJsonObject;

  /**
   * @brief       返回组件是否可以加载。
   *
   * @details     如果组件的依赖项无法解析，或者缺少共享库阻止组件加载，
   *              组件可能加载失败。
   *
   * @returns     如果组件已加载返回 true；否则返回 false。
   *
   */
  [[nodiscard]] auto IsLoaded() const -> bool;

  /**
   * @brief       返回组件的加载状态。
   *
   * @details     返回加载状态的位标志（ComponentLoader::LoadStatus 值）。
   *
   * @returns     包含组件加载状态信息的位字段。
   *
   */
  auto LoadStatus() -> int;

  /**
   * @brief       返回缺失依赖项的列表。
   *
   * @details     任何缺失依赖项的列表，因为这些依赖项在 ComponentLoader 中不可用，
   *              这可用于显示任何缺失依赖项的名称。
   *
   * @returns     缺失依赖项的列表。
   */
  auto MissingDependencies() -> QStringList;

  /**
   * @brief       返回组件的版本。
   *
   * @returns     组件版本。
   */
  auto Version() -> QVersionNumber;

  /**
   * @brief       返回组件的格式化版本字符串。
   *
   * @returns     格式化的版本字符串。
   */
  auto VersionString() -> QString;

  /**
   * @brief       返回组件的反向 DNS 标识符。
   *
   * @returns     标识符。
   *
   */
  auto Identifier() -> QString;

  /**
   * @brief       返回此组件所属的类别。
   *
   * @returns     组件的类别。
   */
  auto Category() -> QString;

  /**
   * @brief       返回组件的供应商。
   *
   * @returns     供应商。
   *
   */
  auto Vendor() -> QString;

  /**
   * @brief       返回组件的许可证文本。
   *
   * @returns     许可证文本。
   */
  auto License() -> QString;

  /**
   * @brief       返回组件的版权信息。
   *
   * @returns     版权文本。
   *
   */
  auto Copyright() -> QString;

  /**
   * @brief       返回组件的描述。
   *
   * @returns     描述文本。
   *
   */
  auto Description() -> QString;

  /**
   * @brief       返回组件的 URL。
   *
   * @returns     URL。
   *
   */
  auto Url() -> QString;

  /**
   * @brief       返回依赖项的字符串列表。
   *
   * @returns     依赖项。
   *
   */
  auto Dependencies() -> QString;

  /**
   * @brief       返回组件是否可以禁用。
   *
   * @details     诸如 Core 之类的组件无法禁用，因为它们对应用程序至关重要。
   *
   * @returns     如果组件可以禁用返回 true；否则返回 false。
   *
   */
  auto CanBeDisabled() -> bool;

  /**
   * @brief       验证依赖项。
   *
   * @details     验证所有依赖项以确保它们已加载，并确保加载的版本
   *              满足我们的最低要求。
   */
  auto ValidateDependencies() -> void;

  friend class ComponentLoader;

 private:
  //! @cond

  QString name_;
  QString filename_;
  QList<sss::extsystem::Component*> dependencies_;
  QJsonObject metadata_;
  bool is_loaded_;
  sss::extsystem::ComponentLoader::LoadFlags load_flags_;
  QList<QString> missing_dependencies_;
  QMap<sss::extsystem::Component*, QVersionNumber> dependency_versions_;

  //! @endcond
};
}  // namespace sss::extsystem

Q_DECLARE_METATYPE(sss::extsystem::Component*)
