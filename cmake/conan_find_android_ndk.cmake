# find_ndk.cmake
# 此脚本通过 Conan 自动安装并配置 Android NDK

# 检查是否已经设置了 NDK 路径
if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
  message(STATUS "Android NDK not found, installing via Conan...")
else()
  message(STATUS "Using existing Android NDK: ${CMAKE_ANDROID_NDK}")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)

# 设置 Conan 配置
if(NOT DEFINED CONAN_NDK_VERSION)
  set(CONAN_NDK_VERSION "r25b") # 可以修改为需要的版本
endif()

if(NOT DEFINED CONAN_PROFILE)
  set(CONAN_PROFILE "default") # 使用默认 profile
endif()

# 配置 Conan 安装
conan_cmake_configure(
  REQUIRES
  android-ndk/${CONAN_NDK_VERSION}
  GENERATORS
  cmake_paths
  PROFILE
  ${CONAN_PROFILE})

# 执行 Conan 安装
conan_cmake_install(
  PATH_OR_REFERENCE
  .
  REMOTE
  ${CONAN_REMOTE_FOR_CMAKE}
  PROFILE
  ${CONAN_PROFILE})

# 包含 Conan 生成的文件
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

message(STATUS "CONAN_ANDROID-NDK_ROOT=${CONAN_ANDROID-NDK_ROOT}")

# 从 Conan 包信息中获取 NDK 路径
if(DEFINED CONAN_ANDROID-NDK_ROOT)
  set(CMAKE_ANDROID_NDK ${CONAN_ANDROID-NDK_ROOT}/bin)
  message(STATUS "Found Android NDK via Conan: ${CMAKE_ANDROID_NDK}")
else()
  # 备选方法：尝试从环境变量获取
  if(DEFINED ENV{ANDROID_NDK_PATH})
    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_PATH})
    message(STATUS "Found Android NDK via environment variable: ${CMAKE_ANDROID_NDK}")
  else()
    message(FATAL_ERROR "Failed to locate Android NDK after Conan installation")
  endif()
endif()

# 缓存 NDK 路径以便后续使用
set(CMAKE_ANDROID_NDK
    ${CMAKE_ANDROID_NDK}
    CACHE PATH "Path to Android NDK" FORCE)

if(DEFINED CMAKE_ANDROID_PLATFORM)
  set(ANDROID_PLATFORM ${CMAKE_ANDROID_PLATFORM})
else()
  set(ANDROID_PLATFORM "android-24")
endif()

# 验证 NDK 路径
if(NOT EXISTS "${CMAKE_ANDROID_NDK}")
  message(FATAL_ERROR "Android NDK path is invalid: ${CMAKE_ANDROID_NDK}")
endif()

# 设置工具链文件路径（如果尚未设置）
if(NOT CMAKE_TOOLCHAIN_FILE)
  # 尝试定位 NDK 自带的 CMake 工具链文件
  set(NDK_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")

  if(EXISTS "${NDK_TOOLCHAIN_FILE}")
    set(CMAKE_TOOLCHAIN_FILE
        "${NDK_TOOLCHAIN_FILE}"
        CACHE FILEPATH "Android toolchain file" FORCE)
    message(STATUS "Using Android toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(WARNING "Android toolchain file not found at ${NDK_TOOLCHAIN_FILE}")
  endif()
endif()

message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

