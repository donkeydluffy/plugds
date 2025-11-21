# User specific CMake configurations for this project This file is copied as-is
# during subproject creation. You can add custom CMake commands here, for
# example:

# 设置用户自定义选项，根据需要添加
if(MSVC)
  list(
    APPEND
    PROJECT_COMPILE_OPTIONS
    /utf-8
    /MP)
else()
  list(
    APPEND
    PROJECT_COMPILE_OPTIONS
    -Wall
    -Wextra)
endif()
# list(APPEND PROJECT_COMPILE_OPTIONS -xxx)

if(WIN32)
  # 优先从 Qt5::qmake 导入目标获取 qmake.exe 的绝对路径，从而得到 Qt 的 bin 目录
  get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
  set(_qt_bin_hints "")

  if(QT_QMAKE_EXECUTABLE)
    get_filename_component(_qt_bin_from_qmake "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    message(STATUS "Inferred Qt bin dir from Qt5::qmake: ${_qt_bin_from_qmake}")
    list(APPEND _qt_bin_hints "${_qt_bin_from_qmake}")
  endif()

  # 兜底：从 Qt5::Core 的导入位置推断（通常是 Qt5Core.dll 所在的 bin 目录）
  get_target_property(_qt_core_loc_rel Qt5::Core IMPORTED_LOCATION_RELEASE)
  get_target_property(_qt_core_loc_dbg Qt5::Core IMPORTED_LOCATION_DEBUG)
  set(_qt_core_loc "${_qt_core_loc_rel}")
  if(NOT _qt_core_loc)
    set(_qt_core_loc "${_qt_core_loc_dbg}")
  endif()
  if(NOT _qt_core_loc)
    get_target_property(_qt_core_loc Qt5::Core IMPORTED_LOCATION)
  endif()
  if(_qt_core_loc)
    get_filename_component(_qt_bin_from_core "${_qt_core_loc}" DIRECTORY)
    list(APPEND _qt_bin_hints "${_qt_bin_from_core}")
  endif()

  # 额外 hints：CMAKE_PREFIX_PATH/QTDIR/bin
  foreach(pfx IN LISTS CMAKE_PREFIX_PATH)
    list(APPEND _qt_bin_hints "${pfx}/bin")
  endforeach()

  # Try to find windeployqt in common Qt locations or use PATH
  find_program(
    WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt.exe windeployqt
    HINTS ${_qt_bin_hints})

  if(NOT WINDEPLOYQT_EXECUTABLE)
    message(
      WARNING
        "windeployqt not found. Please ensure Qt bin directory is on PATH or set CMake variable WINDEPLOYQT_EXECUTABLE to the full path."
    )
  else()
    # Output dir for the exe (Release/Debug configuration aware)
    # $<TARGET_FILE:app> is the exe full path
    add_custom_command(
      TARGET app
      POST_BUILD
      # run windeployqt to deploy Qt dlls & plugins beside the exe
      COMMAND "${WINDEPLOYQT_EXECUTABLE}" --release "$<TARGET_FILE:app>"
      COMMENT "Running windeployqt and copying project plugins to the exe folder")
  endif()
endif()
# 设置用户自定义编译定义，根据需要添加
# list(APPEND PROJECT_COMPILE_DEFINITIONS MY_DEFINE=1 MY_FLAG)
