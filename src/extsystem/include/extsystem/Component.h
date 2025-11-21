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
 * @brief       The Component class holds the information about a discovered component.
 *
 * @class       sss::extsystem::Component Component.h <Component>
 */
class COMPONENT_SYSTEM_DLLSPEC Component {
 public:
  /**
   * @brief       Constructs a new Component.
   */
  Component();

  /**
   * @brief       Constructs a new Component with the given information.
   *
   * @param[in]   name the name of the component.
   * @param[in]   filename the filename of the component.
   * @param[in]   metadata the metadata retrieved from the component file.
   */
  Component(const QString& name, const QString& filename, const QJsonObject& metadata);

  /**
   * @brief       Adds a component dependency to this component.
   *
   * @param[in]   dependency      The required dependency
   * @param[in]   versionNumber   The required dependency version
   */
  auto AddDependency(Component* dependency, QVersionNumber version_number) -> void;

  /**
   * @brief       Returns the name of the component.
   *
   * @returns     the component name.
   */
  auto Name() -> QString;

  /**
   * @brief       Returns the file name of the component.
   *
   * @returns     the component filename.
   *
   */
  auto Filename() -> QString;

  /**
   * @brief       Returns the decoded metadata for the component as a JSON object.
   *
   * @returns     The component metadata.
   */
  auto Metadata() -> QJsonObject;

  /**
   * @brief       Returns where the component could be loaded.
   *
   * @details     A component may fail loading if it's dependencies could not be resolved or if a shared
   *              library is missing preventing the component from being loaded.
   *
   * @returns     true if the component is loaded; otherwise false.
   *
   */
  [[nodiscard]] auto IsLoaded() const -> bool;

  /**
   * @brief       Returns the load status of the component.
   *
   * @details     Returns the bit flags of (ComponentLoader::LoadStatus values) the load status.
   *
   * @returns     the bit field containing information about the load state of the component.
   *
   */
  auto LoadStatus() -> int;

  /**
   * @brief       Returns a list of missing dependencies.
   *
   * @details     The list of any missing dependencies, as these will not be available from the
   *              ComponentLoader, this can be used to show the names of any missing dependencies.
   *
   * @returns     The list of missing dependencies.
   */
  auto MissingDependencies() -> QStringList;

  /**
   * @brief       Returns the version of the component.
   *
   * @returns     the component version.
   */
  auto Version() -> QVersionNumber;

  /**
   * @brief       Returns the version of the component as a formatted string.
   *
   * @returns     the formatted version string.
   */
  auto VersionString() -> QString;

  /**
   * @brief       Returns the reverse dns identifier of the component.
   *
   * @returns     the identifier.
   *
   */
  auto Identifier() -> QString;

  /**
   * @brief       Returns the category that this component belongs to.
   *
   * @returns     the category of the component.
   */
  auto Category() -> QString;

  /**
   * @brief       Returns the vendor of the component.
   *
   * @returns     the vendor.
   *
   */
  auto Vendor() -> QString;

  /**
   * @brief       Returns the license text of the component.
   *
   * @returns     the license text.
   */
  auto License() -> QString;

  /**
   * @brief       Returns the copyright information for the component.
   *
   * @returns     the copyright text.
   *
   */
  auto Copyright() -> QString;

  /**
   * @brief       Returns the description of the component.
   *
   * @returns     the description text.
   *
   */
  auto Description() -> QString;

  /**
   * @brief       Returns the url for the component.
   *
   * @returns     the URL.
   *
   */
  auto Url() -> QString;

  /**
   * @brief       Returns the list of dependencies as a string.
   *
   * @returns     the dependencies.
   *
   */
  auto Dependencies() -> QString;

  /**
   * @brief       Returns whether the component can be disabled or not.
   *
   * @details     Components such as Core cannot be disabled as they are critical for the application.
   *
   * @returns     true if the component can be disabled; otherwise false.
   *
   */
  auto CanBeDisabled() -> bool;

  /**
   * @brief       Validates the dependencies.
   *
   * @details     Validates all dependencies to ensure they are loaded and ensures that the loaded version
   *              meets our minimum requirement.
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
