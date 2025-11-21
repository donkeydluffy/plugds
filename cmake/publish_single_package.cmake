# cmake/publish_single_package.cmake

# --- 正在执行脚本 ---
message(STATUS "--- Executing publish_single_package.cmake ---")
message(STATUS "  Subproject path (relative): ${LIBRARY_PATH_ARG}")
message(STATUS "  Conan package name: ${LIBRARY_NAME_ARG}")
message(STATUS "  Conan package conan version: ${LIBRARY_CONAN_VERSION_ARG}")
message(STATUS "  Conan user: ${CONAN_USER_ARG}")
message(STATUS "  Conan channel: ${CONAN_CHANNEL_ARG}")
message(STATUS "  Conan remote: ${CONAN_REMOTE_ARG}")
message(STATUS "  Conan command path: ${CONAN_CMD_PATH_ARG}")
message(STATUS "  Top-level source directory: ${TOP_SOURCE_DIR_ARG}")
message(STATUS "  Build type: ${BUILD_TYPE_ARG}")
message(STATUS "  Host profile: ${CONAN_PROFILE_HOST_ARG}")
message(STATUS "  Build profile: ${CONAN_PROFILE_BUILD_ARG}")
message(STATUS "  Conan user home: ${CONAN_USER_HOME_ARG}")
message(STATUS "  Target OS name: ${TARGET_OS_NAME}")

# 检查必选参数是否定义
if(NOT DEFINED CONAN_CMD_PATH_ARG
   OR NOT DEFINED LIBRARY_PATH_ARG
   OR NOT DEFINED LIBRARY_NAME_ARG
   OR NOT DEFINED LIBRARY_CONAN_VERSION_ARG
   OR NOT DEFINED CONAN_CMD_PATH_ARG
   OR NOT DEFINED TOP_SOURCE_DIR_ARG
   OR NOT DEFINED BUILD_TYPE_ARG
   OR NOT DEFINED CONAN_PROFILE_HOST_ARG
   OR NOT DEFINED CONAN_PROFILE_BUILD_ARG
   OR NOT DEFINED CONAN_USER_HOME_ARG)
  message(FATAL_ERROR "Required arguments for publish_single_package.cmake script are missing.")
endif()

set(LIBRARY_ABS_PATH "${TOP_SOURCE_DIR_ARG}/${LIBRARY_PATH_ARG}")

# 自动检测 Conan 版本
if(NOT DEFINED CONAN_VERSION_TO_USE)
  # 尝试解析 conan --version
  execute_process(
    COMMAND "${CONAN_CMD_PATH_ARG}" --version
    OUTPUT_VARIABLE _conan_version_str
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _conan_version_result)
  if(_conan_version_result EQUAL 0 AND _conan_version_str MATCHES "^Conan version 2")
    message(STATUS "Detected Conan 2.x version: ${_conan_version_str}")
    set(CONAN_VERSION_TO_USE "2")
  elseif(_conan_version_result EQUAL 0 AND _conan_version_str MATCHES "^Conan version 1")
    message(STATUS "Detected Conan 1.x version: ${_conan_version_str}")
    set(CONAN_VERSION_TO_USE "1")
  else()
    message(
      WARNING "Failed to get Conan version using '${CONAN_CMD_PATH_ARG} --version'. Defaulting to Conan 1.x logic.")
    set(CONAN_VERSION_TO_USE "1")
  endif()
endif()

message(STATUS "Publishing logic will use Conan ${CONAN_VERSION_TO_USE}.x.")

# Conan 1.x 发布逻辑
if(CONAN_VERSION_TO_USE STREQUAL "1")
  set(PUBLISHER_MODULE_PATH "${TOP_SOURCE_DIR_ARG}/cmake/conan_publisher_v1.cmake")
  include(${PUBLISHER_MODULE_PATH})
  if(COMMAND conan1_do_create_and_upload)
    conan1_do_create_and_upload(
      "${CONAN_CMD_PATH_ARG}"
      "${LIBRARY_ABS_PATH}"
      "${LIBRARY_NAME_ARG}"
      "${LIBRARY_CONAN_VERSION_ARG}"
      "${CONAN_USER_ARG}"
      "${CONAN_CHANNEL_ARG}"
      "${CONAN_REMOTE_ARG}"
      "${BUILD_TYPE_ARG}"
      "${CONAN_PROFILE_HOST_ARG}"
      "${CONAN_PROFILE_BUILD_ARG}"
      "${CONAN_USER_HOME_ARG}"
      "${TARGET_OS_NAME}")
  else()
    message(FATAL_ERROR "CMake function 'conan1_do_create_and_upload' is not defined.")
  endif()

  # Conan 2.x 发布逻辑
elseif(CONAN_VERSION_TO_USE STREQUAL "2")
  set(PUBLISHER_MODULE_PATH_V2 "${TOP_SOURCE_DIR_ARG}/cmake/conan_publisher_v2.cmake")
  include(${PUBLISHER_MODULE_PATH_V2})
  if(COMMAND conan2_do_create_and_upload)
    conan2_do_create_and_upload(
      "${CONAN_CMD_PATH_ARG}"
      "${LIBRARY_ABS_PATH}"
      "${LIBRARY_NAME_ARG}"
      "${LIBRARY_CONAN_VERSION_ARG}"
      "${CONAN_USER_ARG}"
      "${CONAN_CHANNEL_ARG}"
      "${CONAN_REMOTE_ARG}"
      "${BUILD_TYPE_ARG}"
      "${CONAN_PROFILE_HOST_ARG}"
      "${CONAN_PROFILE_BUILD_ARG}"
      "${CONAN_USER_HOME_ARG}"
      "${TARGET_OS_NAME}")
  else()
    message(FATAL_ERROR "CMake function 'conan2_do_create_and_upload' is not defined.")
  endif()

  # 不支持的版本
else()
  message(FATAL_ERROR "Unsupported Conan version: ${CONAN_VERSION_TO_USE}")
endif()
