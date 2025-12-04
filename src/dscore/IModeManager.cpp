#include "dscore/IModeManager.h"

namespace sss::dscore {

// 构造函数/析构函数体（如果因 QObject 继承而需要）
// 但通常继承 QObject 的纯虚接口至少需要一个 vtable。
// 由于我们有 Q_OBJECT，需要确保 moc 运行。
// 如果使用了 Q_OBJECT，我们可能需要为 IModeManager 准备一个 cpp 文件，或者 CMake 会在头文件位于源码列表中时处理它。
// 注意，如果 IModeManager 继承 QObject，它就不再仅仅是一个接口了，它有基类数据。
// 这样是没问题的。

}  // namespace sss::dscore
