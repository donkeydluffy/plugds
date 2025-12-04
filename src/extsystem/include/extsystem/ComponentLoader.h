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
 * @brief       ComponentLoader 加载发现的组件。
 *
 * @details     通用插件加载器，在给定文件夹中查找兼容组件，并使用元数据
 *              确保任何依赖项都可用，并且所有依赖项都按正确的顺序加载。
 *
 * @class       sss::extsystem::ComponentLoader ComponentLoader.h <ComponentLoader>
 */
class EXT_SYSTEM_DLLSPEC ComponentLoader : public QObject {
 private:
  Q_OBJECT

 public:
  /**
   * @brief       加载状态标志。
   *
   * @details     用于加载状态的位标志。
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
   * @brief       构造一个 ComponentLoader，它是 parent 的子对象。
   *
   * @param[in]   parent 父对象。
   */
  explicit ComponentLoader(QObject* parent = nullptr);

  /**
   * @brief       销毁 ComponentLoader。
   */
  ~ComponentLoader() override;

  /**
   * @brief       将给定文件夹中的所有组件添加到加载列表。
   *
   * @details     搜索给定目录，并将任何可加载的组件添加到要加载的组件列表中。
   *
   * @param[in]   componentFolder 搜索文件夹。
   */
  auto AddComponents(const QString& component_folder) -> void;

  /**
   * @brief       加载所有发现的组件。
   *
   * @param[in]   loadFunction 加载函数是一个回调，允许应用程序有选择地
   *              加载组件，即用户可以禁用某些组件。
   */
  auto LoadComponents(std::function<bool(sss::extsystem::Component*)> load_function = nullptr) -> void;

  /**
   * @brief       返回所有发现的组件列表。
   *
   * @details     返回找到的组件列表，组件是否已加载的状态会更新，如果组件无法加载，
   *              则为每个组件提供错误代码。
   *
   * @returns     组件列表。
   *
   */
  auto Components() -> QList<Component*>;

  /**
   * @brief       卸载所有已加载的组件。
   */
  auto UnloadComponents() -> void;

 private:
  /**
   * @brief       解析已加载组件的依赖项。
   *
   * @details     对于给定的组件，返回必须按顺序加载的组件列表，
   *              以满足所有组件和子组件的依赖项。
   *
   * @param[in]   component 要解析的组件。
   * @param[out]  resolvedList 已排序的组件列表。
   */
  auto resolve(sss::extsystem::Component* component, QList<sss::extsystem::Component*>& resolved_list) -> void;

  /**
   * @brief           解析已加载组件的依赖项。
   *
   * @details         对于给定的组件，返回必须按顺序加载的组件列表，
   *                  以满足所有组件和子组件的依赖项。
   *
   *                  此重载使用列表来标记节点为已处理，这允许我们检测
   *                  循环引用。
   *
   * @param[in]       component 要解析的组件。
   * @param[in,out]   processedList 已处理节点的列表。
   * @param[out]      resolvedList 已排序的组件列表。
   */
  auto resolve(sss::extsystem::Component* component, QList<sss::extsystem::Component*>& resolved_list,
               QList<sss::extsystem::Component*>& processed_list) -> void;

  /**
   * @brief       返回包含已设置标志的字符串。
   *
   * @param[in]   flags 要转换为字符串的标志值。
   *
   * @returns     包含已设置标志列表的字符串。
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
