#pragma once

#if defined(DS_COMPONENT_CORE_EXPORT)
#define DS_CORE_DLLSPEC Q_DECL_EXPORT
#else
#define DS_CORE_DLLSPEC Q_DECL_IMPORT
#endif
