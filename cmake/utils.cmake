# CMake 函数：根据环境变量、已存在的缓存变量或默认值来定义一个新变量。
# 该新变量可以作为缓存变量（用户可在 CMake GUI/ccmake 中进行编辑）， 或者作为全局变量（全局属性，同时也在父作用域中以普通变量的形式存在）。
# 用法: define_new_variable(
# NEW_VAR_NAME           要创建的新变量名称
# ENV_VAR_TO_CHECK       检查的环境变量名称
# DEFAULT_VALUE          如果环境变量和已存在的缓存都未设置时使用的默认值
# TYPE                   缓存变量的类型（例如 STRING, BOOL, PATH, FILEPATH）。 如果使用 FORCE_GLOBAL 则忽略此项。
# DOCSTRING              缓存变量的说明字符串。如果使用 FORCE_GLOBAL 则忽略此项。
# [FORCE_GLOBAL]         可选参数：如果为 TRUE/ON，则创建全局变量（属性）， 而不是缓存变量。
function(
  define_new_variable
  NEW_VAR_NAME
  ENV_VAR_TO_CHECK
  DEFAULT_VALUE
  TYPE
  DOCSTRING)
  set(options FORCE_GLOBAL)
  set(oneValueArgs) # 除主要参数外无其它单一值参数
  set(multiValueArgs)
  cmake_parse_arguments(
    ARGS
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  set(VALUE_TO_SET "")
  set(VALUE_SOURCE "Default")

  # 检查环境变量
  if(DEFINED ENV{${ENV_VAR_TO_CHECK}}
     AND NOT
         "$ENV{${ENV_VAR_TO_CHECK}}"
         STREQUAL
         "")
    set(VALUE_TO_SET "$ENV{${ENV_VAR_TO_CHECK}}")
    set(VALUE_SOURCE "Environment Variable (${ENV_VAR_TO_CHECK})")
    # 如果环境变量未定义，则检查是否已在缓存中定义了 NEW_VAR_NAME
  elseif(
    DEFINED ${NEW_VAR_NAME}
    AND NOT
        "${${NEW_VAR_NAME}}"
        STREQUAL
        "")
    # 如果 NEW_VAR_NAME 已定义（且不为空），则保留该缓存值。这允许用户覆盖默认值。
    set(VALUE_TO_SET "${${NEW_VAR_NAME}}")
    set(VALUE_SOURCE "Cache (user defined or previously set)")
    # 如果既没有环境变量也没有缓存变量，则使用默认值
  else()
    set(VALUE_TO_SET "${DEFAULT_VALUE}")
    set(VALUE_SOURCE "Default")
  endif()

  # 根据确定的 VALUE_TO_SET 定义 NEW_VAR_NAME
  if(ARGS_FORCE_GLOBAL)
    # 设置为全局属性（类似全局变量）。全局属性在 cmake-gui 中不容易查看或修改， 但可以在任意地方访问。
    set_property(GLOBAL PROPERTY ${NEW_VAR_NAME} "${VALUE_TO_SET}")
    message(STATUS "Defined global variable ${NEW_VAR_NAME} = '${VALUE_TO_SET}' (Source: ${VALUE_SOURCE})")
    # 同时在父作用域中设为普通变量，以便在当前 CMakeLists.txt 或子目录中立即使用
    set(${NEW_VAR_NAME}
        "${VALUE_TO_SET}"
        PARENT_SCOPE)
  else()
    # 设置为缓存变量。缓存变量是处理用户可配置选项的常用方式。
    set(${NEW_VAR_NAME}
        "${VALUE_TO_SET}"
        CACHE ${TYPE} "${DOCSTRING} (Source: ${VALUE_SOURCE})" FORCE)
    message(STATUS "Defined cache variable ${NEW_VAR_NAME} = '${${NEW_VAR_NAME}}' (Source: ${VALUE_SOURCE})")
  endif()

endfunction()

function(get_conan_profile_prefix CONAN_HOST_PROFILE_PREFIX CONAN_BUILD_PROFILE_PREFIX)
  set(CONAN_HOST_PROFILE "conan1")
  set(CONAN_BUILD_PROFILE "conan1")

  if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    message(STATUS "Detected CMAKE_SYSTEM_NAME Android")
    set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_android")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
      message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Windows")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_vs2022")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
      message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Linux")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc9")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
      message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Darwin (macOS)")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_macos_arm64")
    else()
      message(
        FATAL_ERROR
          "Unsupported host system for Android build: ${CMAKE_HOST_SYSTEM_NAME}. Only Windows, Linux, and macOS are supported."
      )
    endif()
    # 根据平台决定基础 Profile 名称
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Windows")
    if(CMAKE_GENERATOR MATCHES "Visual Studio 17 2022")
      message(STATUS "Detected CMAKE_GENERATOR Visual Studio 17 2022")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_vs2022")
      set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_vs2022")
    elseif(CMAKE_GENERATOR MATCHES "Visual Studio 15 2017")
      message(STATUS "Detected CMAKE_GENERATOR Visual Studio 15 2017")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_vs2017")
      set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_vs2017")
    else()
      set(CONAN_HOST_PROFILE "default")
      set(CONAN_BUILD_PROFILE "default")
      message(WARNING "Windows: Failed to determine VS version from CMake generator, using default profiles.")
    endif()
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Linux")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13.0")
      if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8)$")
        message(STATUS "Detected CMAKE_HOST_SYSTEM_PROCESSOR ARMv8 (64-bit)")
        set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc13_arm64")
        set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc13_arm64")
      elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
          message(STATUS "Detected CMAKE_SYSTEM_PROCESSOR x86_64 (64-bit)")
          set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc13")
          set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc13")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8)$")
          message(STATUS "Detected CMAKE_SYSTEM_PROCESSOR ARMv8 (64-bit)")
          set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc13_arm64_cross")
          set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc13")
        else()
          message(
            FATAL_ERROR
              "Linux: Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}. Only x86_64 and ARMv8 (aarch64) are supported."
          )
        endif()
      else()
        message(
          FATAL_ERROR
            "Linux: Unsupported processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Only x86_64 and ARMv8 (aarch64) are supported."
        )
      endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "9.0")
      if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8)$")
        message(STATUS "Detected CMAKE_HOST_SYSTEM_PROCESSOR ARMv8 (64-bit)")
        set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc9_arm64")
        set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc9_arm64")
      elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
          message(STATUS "Detected CMAKE_SYSTEM_PROCESSOR x86_64 (64-bit)")
          set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc9")
          set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc9")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|armv8)$")
          message(STATUS "Detected CMAKE_SYSTEM_PROCESSOR ARMv8 (64-bit)")
          set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_gcc9_arm64_cross")
          set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc9")
        else()
          message(
            FATAL_ERROR
              "Linux: Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}. Only x86_64 and ARMv8 (aarch64) are supported."
          )
        endif()
      else()
        message(
          FATAL_ERROR
            "Linux: Unsupported processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Only x86_64 and ARMv8 (aarch64) are supported."
        )
      endif()
    else()
      message(
        FATAL_ERROR
          "Unsupported compiler or version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}. Only GCC 13+ and Clang 16+ are supported."
      )
    endif()
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")  # 目前只支持linux(ubuntu24.04)上交叉编译macOS arm64; macOS上编译时，不支持交叉编译
    message(STATUS "Detected CMAKE_SYSTEM_NAME Darwin (macOS)")
    set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_macos")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin") # 主机为macos
      message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Darwin")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_macos")

      if (NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL CMAKE_SYSTEM_PROCESSOR)
        message(
          FATAL_ERROR
            "macOS: Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}. must be same in macOS host machine."
        )
      endif()
      # Detect macOS architecture and compiler
      if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
        message(STATUS "Detected CMAKE_HOST_SYSTEM_PROCESSOR x86_64")
        set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_x86_64")
        set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_x86_64")
      elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|armv8)$")
        message(STATUS "Detected CMAKE_HOST_SYSTEM_PROCESSOR arm64")
        set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_arm64")
        set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_arm64")
      else()
        message(
          FATAL_ERROR
            "macOS: Unsupported processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Only x86_64 and arm64 are supported."
        )
      endif()

    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux") # 主机为linux,根据目标架构不同选取不同host profile,且linux一定为x86(不考虑arm64情况)

      message(STATUS "Detected CMAKE_HOST_SYSTEM_NAME Linux")
      set(CONAN_BUILD_PROFILE "${CONAN_BUILD_PROFILE}_gcc13")
      if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|armv8)$")
        message(
          FATAL_ERROR
            "macOS: Unsupported host processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Only x86_64 are supported on linux host."
        )
      endif()
      
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|armv8)$")
        message(STATUS "Detected CMAKE_HOST_SYSTEM_PROCESSOR arm64")
        set(CONAN_HOST_PROFILE "${CONAN_HOST_PROFILE}_arm64_cross")
      else()
        message(
          FATAL_ERROR
            "macOS: Unsupported processor architecture for Darwin cross build: ${CMAKE_SYSTEM_PROCESSOR}. Only arm64 are supported."
        )
      endif()
    else()
      message(
        FATAL_ERROR
          "Unsupported host system for Darwin build: ${CMAKE_HOST_SYSTEM_NAME}. Only Linux, and macOS are supported."
      )
    endif()
  else()
    message(
      FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}. Only Windows, Linux, macOS, and Android are supported.")
  endif()
  set(${CONAN_HOST_PROFILE_PREFIX}
      "${CONAN_HOST_PROFILE}"
      PARENT_SCOPE)
  set(${CONAN_BUILD_PROFILE_PREFIX}
      "${CONAN_BUILD_PROFILE}"
      PARENT_SCOPE)
endfunction()

# 添加Conan包
function(
  add_conan_package
  PACKAGE_NAME
  FIND_PACKAGE_NAME
  PACKAGE_OPTIONS)

  # 解析可选参数
  set(options "")
  set(oneValueArgs PLATFORM)
  set(multiValueArgs "")
  cmake_parse_arguments(
    ARG
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  # 提取基础包名
  string(
    REGEX
    REPLACE "/.*"
            ""
            PACKAGE_BASE_NAME
            "${PACKAGE_NAME}")

  # 处理选项列表
  set(PACKAGE_OPTION_STRING_LIST "")
  if(NOT
     "${PACKAGE_OPTIONS}"
     STREQUAL
     "")
    foreach(OPT IN LISTS PACKAGE_OPTIONS)
      set(PACKAGE_OPTION_STRING "${PACKAGE_BASE_NAME}:${OPT}")
      list(APPEND PACKAGE_OPTION_STRING_LIST "${PACKAGE_OPTION_STRING}")
    endforeach()
  endif()

  string(TOUPPER ${PACKAGE_BASE_NAME} package_base_name_upper)

  # 根据平台参数选择合适的变量名后缀
  if(ARG_PLATFORM)
    string(TOUPPER ${ARG_PLATFORM} platform_upper)
    set(PLATFORM_SUFFIX "_${platform_upper}")
    message(STATUS "Adding platform-specific package for ${ARG_PLATFORM}: ${PACKAGE_NAME}")
  else()
    set(PLATFORM_SUFFIX "")
    message(STATUS "Adding cross-platform package: ${PACKAGE_NAME}")
  endif()

  # 构建变量名
  set(CONAN_PKGS_VAR "CONAN_PKGS${PLATFORM_SUFFIX}")
  set(CONAN_PKG_OPTIONS_VAR "CONAN_PKG_OPTIONS${PLATFORM_SUFFIX}")

  # 更新对应的 CONAN_PKGS 变量
  if(DEFINED ${CONAN_PKGS_VAR})
    list(APPEND ${CONAN_PKGS_VAR} "${PACKAGE_NAME}")
  else()
    set(${CONAN_PKGS_VAR} "${PACKAGE_NAME}")
  endif()
  set(${CONAN_PKGS_VAR}
      ${${CONAN_PKGS_VAR}}
      PARENT_SCOPE)

  # 设置 ${package_base_name_upper}_CONAN_PKG (根据平台区分)
  set(${package_base_name_upper}_CONAN_PKG${PLATFORM_SUFFIX}
      "${PACKAGE_NAME}"
      PARENT_SCOPE)

  if(NOT
     "${PACKAGE_OPTIONS}"
     STREQUAL
     "")
    # 更新对应的 CONAN_PKG_OPTIONS 变量
    if(DEFINED ${CONAN_PKG_OPTIONS_VAR})
      list(APPEND ${CONAN_PKG_OPTIONS_VAR} ${PACKAGE_OPTION_STRING_LIST})
    else()
      set(${CONAN_PKG_OPTIONS_VAR} ${PACKAGE_OPTION_STRING_LIST})
    endif()
    set(${CONAN_PKG_OPTIONS_VAR}
        ${${CONAN_PKG_OPTIONS_VAR}}
        PARENT_SCOPE)

    # 设置 ${package_base_name_upper}_CONAN_PKG_OPTIONS (根据平台区分)
    set(${package_base_name_upper}_CONAN_PKG_OPTIONS${PLATFORM_SUFFIX}
        "${PACKAGE_OPTION_STRING_LIST}"
        PARENT_SCOPE)
  endif()

  # 设置 ${package_base_name_upper}_FIND_PACKAGE_NAME (根据平台区分)
  set(${package_base_name_upper}_FIND_PACKAGE_NAME${PLATFORM_SUFFIX}
      "${FIND_PACKAGE_NAME}"
      PARENT_SCOPE)

  # 输出日志
  message(STATUS "Updated ${CONAN_PKGS_VAR}: ${${CONAN_PKGS_VAR}}")
  if(NOT
     "${PACKAGE_OPTIONS}"
     STREQUAL
     "")
    message(STATUS "Updated ${CONAN_PKG_OPTIONS_VAR}: ${${CONAN_PKG_OPTIONS_VAR}}")
  endif()
endfunction()
