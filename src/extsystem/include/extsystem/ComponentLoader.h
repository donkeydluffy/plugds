#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <functional>

#include "extsystem/ComponentSystemSpec.h"

class QPluginLoader;

namespace sss::extsystem {
class Component;

/**
 * @brief       The ComponentLoader loads the discovered components.
 *
 * @details     Generic plugin loader, finds compatible components in a given folder and using the metadata
 *              ensures that any dependencies are available and that all dependencies are loaded in the correct
 *              order.
 *
 * @class       sss::extsystem::ComponentLoader ComponentLoader.h <ComponentLoader>
 */
class EXT_SYSTEM_DLLSPEC ComponentLoader : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       The load status flags.
   *
   * @details     Bit flags for load status.
   */
  enum LoadFlag {  // NOLINT
    kUnloaded = 0,
    kLoaded = 1,
    kIncompatibleQtVersion = 2,
    kNameClash = 4,
    kMissingDependency = 8,
    kDisabled = 16,
    kIncompatibleVersion = 32,
    kUnableToLoad = 64,
    kMissingInterface = 128,
    kCircularDependency = 256
  };
  Q_ENUM(LoadFlag)
  Q_DECLARE_FLAGS(LoadFlags, LoadFlag)
  Q_FLAGS(LoadFlags)

  /**
   * @brief       Constructs a ComponentLoader which is a child of the parent.
   *
   * @param[in]   parent the parent object.
   */
  explicit ComponentLoader(QObject* parent = nullptr);

  /**
   * @brief       Destroys the ComponentLoader.
   */
  ~ComponentLoader() override;

  /**
   * @brief       Add all components in the given folder to the load list.
   *
   * @details     Searches the given directory and adds any loadable components to the list of components
   *              to be loaded.
   *
   * @param[in]   componentFolder the search folder.
   */
  auto AddComponents(const QString& component_folder) -> void;

  /**
   * @brief       Loads all discovered components.
   *
   * @param[in]   loadFunction the load function is a callback that allows the application to selectively
   *              load components, i.e the user can disable certain components.
   */
  auto LoadComponents(std::function<bool(sss::extsystem::Component*)> load_function = nullptr) -> void;

  /**
   * @brief       Returns the list of all discovered components.
   *
   * @details     Returns the list of components that were found, the state of whether the component was
   *              loaded is updated along with an error code for each component if a component could not
   *              be loaded.
   *
   * @returns     the list of components.
   *
   */
  auto Components() -> QList<Component*>;

  /**
   * @brief       Unloads all loaded components.
   */
  auto UnloadComponents() -> void;

 private:
  /**
   * @brief       Resolves the dependencies of the loaded components.
   *
   * @details     For a given component, returns a list of components in the order that they must be loaded
   *              in order to satisfy all component and sub component dependencies.
   *
   * @param[in]   component the component to resolve.
   * @param[out]  resolvedList the ordered list of components.
   */
  auto resolve(sss::extsystem::Component* component, QList<sss::extsystem::Component*>& resolved_list) -> void;

  /**
   * @brief           Resolves the dependencies of the loaded components.
   *
   * @details         For a given component, returns a list of components in the order that they must be
   *                  loaded in order to satisfy all component and sub component dependencies.
   *
   *                  This overload uses a list to mark nodes as already processed, this allows us to detect
   *                  circular references.
   *
   * @param[in]       component the component to resolve.
   * @param[in,out]   processedList list of nodes that have already been processed.
   * @param[out]      resolvedList ordered list of components.
   */
  auto resolve(sss::extsystem::Component* component, QList<sss::extsystem::Component*>& resolved_list,
               QList<sss::extsystem::Component*>& processed_list) -> void;

  /**
   * @brief       Returns a string containing the flags that were set.
   *
   * @param[in]   flags the flags value to be converted to a string.
   *
   * @returns     a string containing the list of flags that were set.
   */
  auto loadFlagString(sss::extsystem::ComponentLoader::LoadFlags flags) -> QString;

  //! @cond

  QList<QPair<QPluginLoader*, sss::extsystem::Component*> > load_order_;
  QMap<QString, sss::extsystem::Component*> component_search_list_;

  //! @endcond
};
}  // namespace sss::extsystem

Q_DECLARE_OPERATORS_FOR_FLAGS(sss::extsystem::ComponentLoader::LoadFlags)
Q_DECLARE_METATYPE(sss::extsystem::ComponentLoader::LoadFlags)
Q_DECLARE_METATYPE(sss::extsystem::ComponentLoader::LoadFlag)
