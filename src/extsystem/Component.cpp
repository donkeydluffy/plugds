#include "extsystem/Component.h"

#include <QJsonArray>

sss::extsystem::Component::Component() : is_loaded_(false), load_flags_(ComponentLoader::kUnloaded) {}

sss::extsystem::Component::Component(const QString& name, const QString& filename,  // NOLINT
                                     const QJsonObject& metadata)                   // NOLINT
    : name_(name),
      filename_(filename),
      metadata_(metadata),
      is_loaded_(false),
      load_flags_(ComponentLoader::kUnloaded) {}

void sss::extsystem::Component::AddDependency(Component* dependency, QVersionNumber version_number) {
  dependency_versions_[dependency] = std::move(version_number);
  dependencies_.append(dependency);
}

auto sss::extsystem::Component::Name() -> QString { return name_; }

auto sss::extsystem::Component::Filename() -> QString { return filename_; }

auto sss::extsystem::Component::Metadata() -> QJsonObject { return metadata_; }

auto sss::extsystem::Component::IsLoaded() const -> bool { return is_loaded_; }

auto sss::extsystem::Component::LoadStatus() -> int { return load_flags_; }

auto sss::extsystem::Component::MissingDependencies() -> QStringList { return missing_dependencies_; }

auto sss::extsystem::Component::Version() -> QVersionNumber {
  auto component_metadata = metadata_["MetaData"].toObject();
  auto component_version = component_metadata["Version"].toString();

  return QVersionNumber::fromString(component_version);
}

auto sss::extsystem::Component::VersionString() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();
  auto component_version = component_metadata["Version"].toString();
  auto component_branch = component_metadata["Branch"].toString();
  auto component_revision = component_metadata["Revision"].toString();

  return QString("%1-%2 (%3)").arg(component_version).arg(component_branch).arg(component_revision);
}

auto sss::extsystem::Component::Identifier() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();

  return (component_metadata["Name"].toString() + "." + component_metadata["Vendor"].toString()).toLower();
}

auto sss::extsystem::Component::Category() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();

  return component_metadata["Category"].toString();
}

auto sss::extsystem::Component::Vendor() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();

  return component_metadata["Vendor"].toString();
}

auto sss::extsystem::Component::License() -> QString {
  QString license_text;
  auto component_metadata = metadata_["MetaData"].toObject();

  auto license_content = component_metadata["License"].toArray();

  for (auto object : license_content) {
    license_text += object.toString();
  }

  return license_text;
}

auto sss::extsystem::Component::Copyright() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();

  return component_metadata["Copyright"].toString();
}

auto sss::extsystem::Component::Description() -> QString {
  QString description_text;
  auto component_metadata = metadata_["MetaData"].toObject();

  auto license_content = component_metadata["Description"].toArray();

  for (auto object : license_content) {
    description_text += object.toString() + "\r\n";
  }

  return description_text;
}

auto sss::extsystem::Component::Url() -> QString {
  auto component_metadata = metadata_["MetaData"].toObject();

  return component_metadata["Url"].toString();
}

auto sss::extsystem::Component::Dependencies() -> QString {
  QString dependency_text;
  auto component_metadata = metadata_["MetaData"].toObject();

  auto license_content = component_metadata["Dependencies"].toArray();

  for (auto object : license_content) {
    auto dependency = object.toObject();

    dependency_text += QString("%1 (%2)\r\n").arg(dependency["Name"].toString()).arg(dependency["Version"].toString());
  }

  return dependency_text;
}

auto sss::extsystem::Component::CanBeDisabled() -> bool {
  auto component_metadata = metadata_["MetaData"].toObject();

  if (component_metadata.contains("CanBeDisabled")) {
    return component_metadata["CanBeDisabled"].toBool();
  }

  return true;
}

auto sss::extsystem::Component::ValidateDependencies() -> void {
  for (auto* dependency : dependencies_) {
    if (!dependency->IsLoaded()) {
      load_flags_ |= sss::extsystem::ComponentLoader::kMissingDependency;
    } else {
      if (dependency->Version() < dependency_versions_[dependency]) {
        load_flags_ |= sss::extsystem::ComponentLoader::kIncompatibleVersion;
      }
    }
  }
}
