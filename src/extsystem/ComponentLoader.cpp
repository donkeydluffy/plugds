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
    SPDLOG_WARN(QString("Application was built with QT_DEBUG but has loaded RELEASE qt libraries, component system "
                        "will load DEBUG components instead.")
                    .toStdString());

    application_debug_build = true;
  }
#else
  if (application_debug_build) {
    SPDLOG_WARN(tr("Application was built with QT_NO_DEBUG but has loaded DEBUG qt libraries, component system will "
                   "load RELEASE components instead.")
                    .toStdString());

    application_debug_build = false;
  }
#endif
#endif
  // SPDLOG_INFO(QString("Searching folder for components %1").arg(component_folder).toStdString());

  SPDLOG_INFO(QString("Searching folder %1 for components").arg(component_folder).toStdString());

  QDirIterator dir(component_folder);

  // find compatible components, and create a list of components to consider for loading

  while (dir.hasNext()) {
    dir.next();

    auto component_filename = dir.fileInfo().absoluteFilePath();

    SPDLOG_INFO(QString("Found file: %1 (isDir: %2)")
                    .arg(component_filename)
                    .arg(dir.fileInfo().isDir() ? "true" : "false")
                    .toStdString());

    if (dir.fileInfo().isDir()) {
      SPDLOG_INFO(QString("Skipping directory: %1").arg(component_filename).toStdString());
      continue;
    }

    bool is_lib = QLibrary::isLibrary(component_filename);
    SPDLOG_INFO(QString("File %1 isLibrary: %2").arg(component_filename).arg(is_lib ? "true" : "false").toStdString());

    if (!is_lib) {
      SPDLOG_INFO(QString("Skipping non-library file: %1").arg(component_filename).toStdString());
      continue;
    }

    auto* plugin_loader = new QPluginLoader(component_filename);

    SPDLOG_INFO(QString("Processing library: %1").arg(component_filename).toStdString());

    auto meta_data_object = plugin_loader->metaData();

    if (meta_data_object.isEmpty()) {
      SPDLOG_INFO(QString("Library %1 has empty metadata").arg(component_filename).toStdString());
      delete plugin_loader;
      continue;
    }

    SPDLOG_INFO(QString("Library %1 has metadata").arg(component_filename).toStdString());

    auto debug_build = meta_data_object.value("debug");
    auto qt_version = meta_data_object.value("version");

    SPDLOG_INFO(QString("Library %1 metadata keys: %2")
                    .arg(component_filename)
                    .arg(QStringList(meta_data_object.keys()).join(","))
                    .toStdString());

    // Try to get component metadata from both "MetaData" and directly in the root
    auto component_metadata = meta_data_object.value("MetaData");

    // If MetaData doesn't exist, use the root metadata object
    if (component_metadata.isNull() || component_metadata.isUndefined()) {
      component_metadata = QJsonValue(meta_data_object);
      SPDLOG_INFO(
          QString("Library %1 using root metadata as component metadata").arg(component_filename).toStdString());
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

    SPDLOG_INFO(QString("Library %1 MetaData type: %2").arg(component_filename).arg(metadata_type).toStdString());
    SPDLOG_INFO(QString("Library %1 has debug flag: %2")
                    .arg(component_filename)
                    .arg(!debug_build.isNull() ? "true" : "false")
                    .toStdString());
    SPDLOG_INFO(QString("Library %1 has version flag: %2")
                    .arg(component_filename)
                    .arg(!qt_version.isNull() ? "true" : "false")
                    .toStdString());

    if (debug_build.isNull() || qt_version.isNull() || (component_metadata.type() != QJsonValue::Object)) {
      SPDLOG_INFO(QString("Library %1 missing required metadata fields").arg(component_filename).toStdString());
      delete plugin_loader;
      continue;
    }

    SPDLOG_INFO(QString("Library %1 debug flag: %2, application debug: %3")
                    .arg(component_filename)
                    .arg(debug_build.toBool() ? "true" : "false")
                    .arg(application_debug_build ? "true" : "false")
                    .toStdString());

    // Allow loading component even if there's a debug/release mismatch for debugging purposes
    // if (debug_build != application_debug_build) {
    //   SPDLOG_INFO(QString("Library %1 debug flag mismatch").arg(component_filename).toStdString());
    //   delete plugin_loader;
    //   continue;
    // }

    // Still log a warning about the mismatch
    if (debug_build != application_debug_build) {
      SPDLOG_WARN(QString("Component %1 has a debug/release mismatch with the application. This may cause instability.")
                      .arg(component_filename)
                      .toStdString());
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
      SPDLOG_INFO(QString("Library %1 using className as component name: %2")
                      .arg(component_filename)
                      .arg(component_name.isNull() ? "NULL" : component_name.toString())
                      .toStdString());
    }

    SPDLOG_INFO(QString("Library %1 component name: %2")
                    .arg(component_filename)
                    .arg(component_name.isNull() ? "NULL" : component_name.toString())
                    .toStdString());
    SPDLOG_INFO(QString("Library %1 all metadata: %2")
                    .arg(component_filename)
                    .arg(QString(QJsonDocument(component_metadata_obj).toJson()))
                    .toStdString());

    if (component_name.isNull() || component_name.toString().isEmpty()) {
      SPDLOG_INFO(QString("Library %1 missing name").arg(component_filename).toStdString());
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
    SPDLOG_INFO(QString("Library %1 application Qt version: %2, component Qt version: %3")
                    .arg(component_filename)
                    .arg(application_qt_version_str)
                    .arg(component_qt_version_str)
                    .toStdString());

    auto* component = new sss::extsystem::Component(component_name.toString(), component_filename, meta_data_object);

    connect(this, &sss::extsystem::ComponentLoader::destroyed, [=](QObject*) { delete component; });

    if (component_qt_version.majorVersion() != application_qt_version.majorVersion()) {
      component->load_flags_.setFlag(LoadFlag::kIncompatibleQtVersion);
      SPDLOG_INFO(QString("Library %1 incompatible Qt version").arg(component_filename).toStdString());
    }

    if (component_search_list_.contains(component_name.toString())) {
      component->load_flags_.setFlag(LoadFlag::kNameClash);
      SPDLOG_INFO(QString("Library %1 name clash").arg(component_filename).toStdString());
    }

    component_search_list_[component_name.toString()] = component;
    SPDLOG_INFO(QString("Library %1 added to component_search_list as %2")
                    .arg(component_filename)
                    .arg(component_name.toString())
                    .toStdString());

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

  for (auto* current : component_load_list) {
    QList<sss::extsystem::Component*> dependency_resolve_list;

    if (!resolved_load_list.contains(current)) {
      dependency_resolve_list.clear();

      resolve(current, dependency_resolve_list);

      for (auto* dependency : dependency_resolve_list) {
        if (!resolved_load_list.contains(dependency)) {
          resolved_load_list.append(dependency);
        }
      }
    }
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
                                              QList<sss::extsystem::Component*>& processed_list) -> void {
  processed_list.append(component);

  for (auto* dependency : component->dependencies_) {
    if (!resolved_list.contains(dependency)) {
      if (processed_list.contains(dependency)) {
        continue;
      }

      resolve(dependency, resolved_list, processed_list);
    }
  }

  resolved_list.append(component);
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
