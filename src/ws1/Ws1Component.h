#pragma once

#include <QObject>
#include <memory>

#include "extsystem/IComponent.h"

namespace sss::dscore {
class ICommandManager;
class ILanguageService;
}  // namespace sss::dscore

namespace sss::ws1 {

class Ws1Page;
class Ws1UIProvider;

class Ws1Component : public QObject, public sss::extsystem::IComponent {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID SSSComponentInterfaceIID FILE "metadata.json")
  Q_INTERFACES(sss::extsystem::IComponent)

 public:
  Ws1Component() = default;
  ~Ws1Component() override = default;

  // IComponent 接口
  void InitialiseEvent() override;
  void InitialisationFinishedEvent() override;
  void FinaliseEvent() override;

 public slots:  // NOLINT
  void UpdateIcons(const QString& theme_id);

 private:
  // 已移除 createSampleCommand 和 createSwitchCommands 方法

  int page_context_id_ = 0;
  int sub_context_id_ = 0;
  Ws1Page* ws1_mode_ = nullptr;  // 跟踪我们的模式实例

  std::unique_ptr<Ws1UIProvider> ui_provider_;
};

}  // namespace sss::ws1
