#include "extsystem/ComponentLoader.h"

#include <spdlog/spdlog.h>

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLibrary>
#include <QLibraryInfo>
#include <QMap>
#include <QMetaEnum>
#include <QPluginLoader>
#include <QtGlobal>

#include "extsystem/Component.h"
#include "extsystem/IComponent.h"

constexpr unsigned int kQtMajorBitMask = 0xFFFF0000;
constexpr unsigned int kQtMajorBitShift = 16;
constexpr unsigned int kQtMinorBitMask = 0x0000FF00;
constexpr unsigned int kQtMinorBitShift = 8;
constexpr unsigned int kQtPatchBitMask = 0x000000FF;
constexpr unsigned int kQtPatchBitShift = 0;

sss::extsystem::ComponentLoader::ComponentLoader(QObject* parent) : QObject(parent) {}

sss::extsystem::ComponentLoader::~ComponentLoader() { UnloadComponents(); }

auto sss::extsystem::ComponentLoader::AddComponents(const QString& component_folder) -> void {
  auto application_debug_build = QLibraryInfo::isDebugBuild();
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
  auto application_qt_version = QLibraryInfo::version();
#else
  auto application_qt_version = QVersionNumber::fromString(qVersion());
#endif

#if defined(Q_OS_UNIX) || ((defined(Q_OS_WIN) && defined(__MINGW32__)))
#if defined(QT_DEBUG)
  if (!application_debug_build) {
    SPDLOG_WARN(
        "Application was built with QT_DEBUG but has loaded RELEASE qt libraries, component system will load DEBUG "
        "components instead.");

    application_debug_build = true;
  }
#else
  if (application_debug_build) {
    SPDLOG_WARN(
        "Application was built with QT_NO_DEBUG but has loaded DEBUG qt libraries, component system will load RELEASE "
        "components instead.");

    application_debug_build = false;
  }
#endif
#endif
  // SPDLOG_INFO(QString("Searching folder for components %1").arg(component_folder).toStdString());

  SPDLOG_INFO("Searching folder {} for components", component_folder.toStdString());

  QDirIterator dir(component_folder);

  // find compatible components, and create a list of components to consider for loading

  while (dir.hasNext()) {
    dir.next();

    auto component_filename = dir.fileInfo().absoluteFilePath();

    SPDLOG_INFO("Found file: {} (isDir: {})", component_filename.toStdString(),
                dir.fileInfo().isDir() ? "true" : "false");

    if (dir.fileInfo().isDir()) {
      SPDLOG_INFO("Skipping directory: {}", component_filename.toStdString());
      continue;
    }

    bool is_lib = QLibrary::isLibrary(component_filename);
    SPDLOG_INFO("File {} isLibrary: {}", component_filename.toStdString(), is_lib ? "true" : "false");

    if (!is_lib) {
      SPDLOG_INFO("Skipping non-library file: {}", component_filename.toStdString());
      continue;
    }

    auto* plugin_loader = new QPluginLoader(component_filename);

    SPDLOG_INFO("Processing library: {}", component_filename.toStdString());

    auto meta_data_object = plugin_loader->metaData();

    if (meta_data_object.isEmpty()) {
      SPDLOG_INFO("Library {} has empty metadata", component_filename.toStdString());
      delete plugin_loader;
      continue;
    }

    SPDLOG_INFO("Library {} has metadata", component_filename.toStdString());

    auto debug_build = meta_data_object.value("debug");
    auto qt_version = meta_data_object.value("version");

    SPDLOG_INFO("Library {} metadata keys: {}", component_filename.toStdString(),
                QStringList(meta_data_object.keys()).join(",").toStdString());

    // Try to get component metadata from both "MetaData" and directly in the root
    auto component_metadata = meta_data_object.value("MetaData");

    // If MetaData doesn't exist, use the root metadata object
    if (component_metadata.isNull() || component_metadata.isUndefined()) {
      component_metadata = QJsonValue(meta_data_object);
      SPDLOG_INFO("Library {} using root metadata as component metadata", component_filename.toStdString());
    }

    // Check the actual type of component_metadata
    QString metadata_type;
    switch (component_metadata.type()) {
      case QJsonValue::Null:
        metadata_type = "Null";
        break;
      case QJsonValue::Bool:
        metadata_type = "Bool";
        break;
      case QJsonValue::Double:
        metadata_type = "Double";
        break;
      case QJsonValue::String:
        metadata_type = "String";
        break;
      case QJsonValue::Array:
        metadata_type = "Array";
        break;
      case QJsonValue::Object:
        metadata_type = "Object";
        break;
      case QJsonValue::Undefined:
        metadata_type = "Undefined";
        break;
      default:
        metadata_type = "Unknown";
        break;
    }

    SPDLOG_INFO("Library {} MetaData type: {}", component_filename.toStdString(), metadata_type.toStdString());
    SPDLOG_INFO("Library {} has debug flag: {}", component_filename.toStdString(),
                !debug_build.isNull() ? "true" : "false");
    SPDLOG_INFO("Library {} has version flag: {}", component_filename.toStdString(),
                !qt_version.isNull() ? "true" : "false");

    if (debug_build.isNull() || qt_version.isNull() || (component_metadata.type() != QJsonValue::Object)) {
      SPDLOG_INFO("Library {} missing required metadata fields", component_filename.toStdString());
      delete plugin_loader;
      continue;
    }

    SPDLOG_INFO("Library {} debug flag: {}, application debug: {}", component_filename.toStdString(),
                debug_build.toBool() ? "true" : "false", application_debug_build ? "true" : "false");

    // Allow loading component even if there's a debug/release mismatch for debugging purposes
    // if (debug_build != application_debug_build) {
    //   SPDLOG_INFO(QString("Library %1 debug flag mismatch").arg(component_filename).toStdString());
    //   delete plugin_loader;
    //   continue;
    // }

    // Still log a warning about the mismatch
    if (debug_build != application_debug_build) {
      SPDLOG_WARN("Component {} has a debug/release mismatch with the application. This may cause instability.",
                  component_filename.toStdString());
    }

    // Check for both "Name" and "name" since case sensitivity might be an issue
    auto component_metadata_obj = component_metadata.toObject();
    auto component_name = component_metadata_obj.value("Name");

    // If "Name" doesn't exist or is empty, try "name" lowercase
    if (component_name.isNull() || component_name.toString().isEmpty()) {
      component_name = component_metadata_obj.value("name");
    }

    // If still no name, use the className from the plugin metadata
    if (component_name.isNull() || component_name.toString().isEmpty()) {
      component_name = component_metadata_obj.value("className");
      SPDLOG_INFO("Library {} using className as component name: {}", component_filename.toStdString(),
                  component_name.isNull() ? "NULL" : component_name.toString().toStdString());
    }

    SPDLOG_INFO("Library {} component name: {}", component_filename.toStdString(),
                component_name.isNull() ? "NULL" : component_name.toString().toStdString());
    SPDLOG_INFO("Library {} all metadata: {}", component_filename.toStdString(),
                QString(QJsonDocument(component_metadata_obj).toJson()).toStdString());

    if (component_name.isNull() || component_name.toString().isEmpty()) {
      SPDLOG_INFO("Library {} missing name", component_filename.toStdString());
      delete plugin_loader;
      continue;
    }

    auto component_qt_major = static_cast<int>((qt_version.toVariant().toUInt() & kQtMajorBitMask) >> kQtMajorBitShift);
    auto component_qt_minor = static_cast<int>((qt_version.toVariant().toUInt() & kQtMinorBitMask) >> kQtMinorBitShift);
    auto component_qt_patch = static_cast<int>((qt_version.toVariant().toUInt() & kQtPatchBitMask) >> kQtPatchBitShift);

    auto component_qt_version = QVersionNumber(component_qt_major, component_qt_minor, component_qt_patch);

    auto application_qt_version_str = QString("%1.%2.%3")
                                          .arg(application_qt_version.majorVersion())
                                          .arg(application_qt_version.minorVersion())
                                          .arg(application_qt_version.microVersion());
    auto component_qt_version_str = QString("%1.%2.%3")
                                        .arg(component_qt_version.majorVersion())
                                        .arg(component_qt_version.minorVersion())
                                        .arg(component_qt_version.microVersion());
    SPDLOG_INFO("Library {} application Qt version: {}, component Qt version: {}", component_filename.toStdString(),
                application_qt_version_str.toStdString(), component_qt_version_str.toStdString());

    auto* component = new sss::extsystem::Component(component_name.toString(), component_filename, meta_data_object);

    connect(this, &sss::extsystem::ComponentLoader::destroyed, [=](QObject*) { delete component; });

    if (component_qt_version.majorVersion() != application_qt_version.majorVersion()) {
      component->load_flags_.setFlag(LoadFlag::kIncompatibleQtVersion);
      SPDLOG_INFO("Library {} incompatible Qt version", component_filename.toStdString());
    }

    if (component_search_list_.contains(component_name.toString())) {
      component->load_flags_.setFlag(LoadFlag::kNameClash);
      SPDLOG_INFO("Library {} name clash", component_filename.toStdString());
    }

    component_search_list_[component_name.toString()] = component;
    SPDLOG_INFO("Library {} added to component_search_list as {}", component_filename.toStdString(),
                component_name.toString().toStdString());

    delete plugin_loader;
  }
}

auto sss::extsystem::ComponentLoader::LoadComponents(
    std::function<bool(sss::extsystem::Component*)> load_function)  // NOLINT
    -> void {
  QList<sss::extsystem::Component*> component_load_list;
  QList<sss::extsystem::Component*> resolved_load_list;

  // find and add dependencies from the search

  auto component_iterator = QMapIterator<QString, sss::extsystem::Component*>(component_search_list_);

  while (component_iterator.hasNext()) {
    component_iterator.next();

    auto* component = component_iterator.value();

    if (component->load_flags_ != 0) {
      continue;
    }

    auto metadata = component->Metadata().value("MetaData").toObject();

    auto dependencies = metadata.value("Dependencies").toArray();

    for (auto dependency : dependencies) {
      auto dependency_name = dependency.toObject().value("Name").toString();
      auto dependency_version = dependency.toObject().value("Version").toString();

      if (component_search_list_.contains(dependency_name)) {
        component->AddDependency(component_search_list_[dependency_name],
                                 QVersionNumber::fromString(dependency_version));
      } else {
        component->missing_dependencies_.append(dependency_name);
        component->load_flags_ |= LoadFlag::kMissingDependency;
      }
    }

    if (!component->load_flags_) {
      component_load_list.append(component);
    }
  }

  // resolve the dependencies to create a load order

  // Clear existing resolved_load_list to ensure a fresh start for dependency resolution
  resolved_load_list.clear();

  QList<sss::extsystem::Component*>
      processed_list_global;  // Use a global processed list for cycle detection across all calls

  for (auto* current : component_load_list) {
    if (!resolved_load_list.contains(current) && !processed_list_global.contains(current)) {
      QList<sss::extsystem::Component*> current_processed_list;  // This processed_list is for current recursion stack
      resolve(current, resolved_load_list, current_processed_list);
    }
  }

  SPDLOG_INFO("Final component load order:");
  for (auto* component : resolved_load_list) {
    SPDLOG_INFO("- {}", component->Name().toStdString());
  }

  // load the components that we have satisfied dependencies for

  for (auto* component : resolved_load_list) {
    if (component->load_flags_ != 0) {
      SPDLOG_WARN("Component {} was not loaded because of pre-existing flags: {}", component->Name().toStdString(),
                  loadFlagString(component->load_flags_).toStdString());
      continue;
    }

    component->ValidateDependencies();

    if (component->load_flags_ != 0) {
      SPDLOG_WARN("Component {} was not loaded after dependency validation. Flags: {}", component->Name().toStdString(),
                  loadFlagString(component->load_flags_).toStdString());
      continue;
    }

    if (load_function) {
      if (!load_function(component)) {
        component->load_flags_.setFlag(sss::extsystem::ComponentLoader::kDisabled);

        SPDLOG_INFO("Component {} was not loaded because it is disabled by configuration.",
                    component->Name().toStdString());
        continue;
      }
    }

    // check if dependencies are loaded, if not then this component cannot be loaded

    auto* plugin_loader = new QPluginLoader(component->Filename());

    if (!plugin_loader->load()) {
      component->load_flags_.setFlag(sss::extsystem::ComponentLoader::kUnableToLoad);

      SPDLOG_ERROR("Component {} was not loaded. Unable to load library. Error: {}", component->Name().toStdString(),
                   plugin_loader->errorString().toStdString());
      delete plugin_loader;

      continue;
    }

    auto* component_interface = qobject_cast<sss::extsystem::IComponent*>(plugin_loader->instance());

    if (component_interface == nullptr) {
      component->load_flags_.setFlag(sss::extsystem::ComponentLoader::kMissingInterface);
      delete plugin_loader;

      SPDLOG_ERROR("Component {} was not loaded. The library does not export a valid IComponent interface.",
                   component->Name().toStdString());
      continue;
    }

    component->load_flags_.setFlag(sss::extsystem::ComponentLoader::kLoaded);

    load_order_.append(QPair<QPluginLoader*, Component*>(plugin_loader, component));

    component->is_loaded_ = true;
  }

  // call initialiseEvent for each component (in load order)

  for (auto component_pair : load_order_) {
    auto* component_interface = qobject_cast<sss::extsystem::IComponent*>(component_pair.first->instance());

    component_interface->InitialiseEvent();
  }

  // call initialisationFinishedEvent for each component (in reverse load order)

  for (auto loaded_component_iterator = load_order_.rbegin(); loaded_component_iterator < load_order_.rend();
       loaded_component_iterator++) {
    auto* component_interface = qobject_cast<sss::extsystem::IComponent*>(loaded_component_iterator->first->instance());

    component_interface->InitialisationFinishedEvent();
  }
}

auto sss::extsystem::ComponentLoader::Components() -> QList<sss::extsystem::Component*> {
  return component_search_list_.values();
}

auto sss::extsystem::ComponentLoader::resolve(sss::extsystem::Component* component,
                                              QList<sss::extsystem::Component*>& resolved_list) -> void {
  QList<sss::extsystem::Component*> processed_list;

  resolve(component, resolved_list, processed_list);
}

auto sss::extsystem::ComponentLoader::resolve(sss::extsystem::Component* component,
                                              QList<sss::extsystem::Component*>& resolved_list,
                                              QList<sss::extsystem::Component*>& processed_list_current_branch)
    -> void {
  // If component is already in the final resolved list, skip it.
  if (resolved_list.contains(component)) {
    return;
  }

  // If component is already in the current processing branch, it's a circular dependency.
  if (processed_list_current_branch.contains(component)) {
    component->load_flags_.setFlag(LoadFlag::kCircularDependency);
    SPDLOG_ERROR("Circular dependency detected involving component: {}", component->Name().toStdString());
    return;  // Break this branch of recursion
  }

  processed_list_current_branch.append(component);  // Mark component as currently being processed

  for (auto* dependency : component->dependencies_) {
    // Only resolve dependency if it's not already resolved
    if (!resolved_list.contains(dependency)) {
      resolve(dependency, resolved_list, processed_list_current_branch);
    }
  }

  // After resolving all its dependencies (or handling circular ones), add the component
  // if it's not already resolved and not marked with a circular dependency.
  if (!resolved_list.contains(component) && !component->load_flags_.testFlag(LoadFlag::kCircularDependency)) {
    resolved_list.append(component);
  }

  processed_list_current_branch.removeAll(component);  // Remove from current processing branch once done
}

auto sss::extsystem::ComponentLoader::UnloadComponents() -> void {
  for (auto loaded_component_iterator = load_order_.rbegin(); loaded_component_iterator < load_order_.rend();
       loaded_component_iterator++) {
    auto* plugin_loader = qobject_cast<QPluginLoader*>(loaded_component_iterator->first);

    auto* component_interface = qobject_cast<sss::extsystem::IComponent*>(plugin_loader->instance());

    if (component_interface == nullptr) {
      continue;
    }

    component_interface->FinaliseEvent();

    if (plugin_loader != nullptr) {
#if !defined(Q_OS_MACOS)
      /**
       * NOTE: calling unload on macOS causes the application to crash, so we leak the memory as it's of no
       * consequence as all memory will be freed when we exit.  For Windows and Linux we include the unload
       * for completeness and to stop memory leak analysers reporting a leak.
       */
      plugin_loader->unload();
#endif
      delete plugin_loader;
    }
  }

  load_order_.clear();
}

auto sss::extsystem::ComponentLoader::loadFlagString(sss::extsystem::ComponentLoader::LoadFlags flags)  // NOLINT
    -> QString {
  auto meta_enum = QMetaEnum::fromType<sss::extsystem::ComponentLoader::LoadFlag>();

  return QString::fromLatin1(meta_enum.valueToKeys(flags)).replace("|", " | ");
}
