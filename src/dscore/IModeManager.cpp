#include "dscore/IModeManager.h"

namespace sss::dscore {

// Constructor/Destructor body if needed for QObject inheritance
// But typically pure virtual interface with QObject inheritance requires at least a vtable.
// Since we have Q_OBJECT, we need to make sure moc runs.
// And we might need a cpp file for IModeManager if Q_OBJECT is used, or CMake handles it if header is in sources list.
// Wait, if IModeManager inherits QObject, it's not just an interface anymore, it has base class data.
// This is fine.

}  // namespace sss::dscore
