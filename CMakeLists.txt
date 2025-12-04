# CMakeLists.txt (根目录)
cmake_minimum_required(VERSION 3.23) # 与 CMakePresets.json 中的版本保持一致或更高

message(STATUS "Current CMake generator: ${CMAKE_GENERATOR}")
message(STATUS "CONAN_USER_FROM_CI: $ENV{CONAN_USER_FROM_CI}, CONAN_CHANNEL_FROM_CI: $ENV{CONAN_CHANNEL_FROM_CI}")
message(
  STATUS
    "PRODUCT_VERSION_FROM_CI: $ENV{PRODUCT_VERSION_FROM_CI}, PRODUCT_VERSION_REVISION_FROM_CI: $ENV{PRODUCT_VERSION_REVISION_FROM_CI}"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR})

# 初始化 Conan 环境
include(conan_env_setup)

if(DEFINED CMAKE_SYSTEM_NAME AND CMAKE_SYSTEM_NAME STREQUAL "Android")
  # 为了外层能够正确拉取Android平台的包
  set(ENV{CONAN_TARGET_OS_NAME} "${CMAKE_SYSTEM_NAME}")
  include(${CMAKE_SOURCE_DIR}/cmake/conan_find_android_ndk.cmake)
endif()

project(
  MyCppProjectManager
  LANGUAGES CXX
  VERSION 1.0.0)

# Configure QT_NO_DEBUG for non-debug builds
# For multi-configuration generators (MSVC, Xcode)
add_compile_definitions($<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG>)
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

# For single-configuration generators (Makefile, Ninja)
if(NOT
   CMAKE_BUILD_TYPE
   STREQUAL
   "Debug")
  add_compile_definitions(QT_NO_DEBUG)
endif()

# 指定的库版本
include(utils)
# 库版本 一般从 CI/CD 环境变量中获取，只有tag才会根据tag号生成版本，否则使用这里的默认版本号
define_new_variable(
  PRODUCT_VERSION
  PRODUCT_VERSION_FROM_CI
  "0.1.0"
  STRING
  "Version of the package to be built.")
if(NOT DEFINED ENV{PRODUCT_VERSION_FROM_CI} OR "$ENV{PRODUCT_VERSION_FROM_CI}" STREQUAL "")
  # 库的修订版本，CI/CD 中一般使用代码提交哈希号，本地开发时可以使用时间戳
  string(TIMESTAMP DEFAULT_PRODUCT_VERSION_REVISION "%Y%m%d%H%M%S")
  message(STATUS "Time-based default PRODUCT_VERSION_REVISION: ${DEFAULT_PRODUCT_VERSION_REVISION}")
  define_new_variable(
    PRODUCT_VERSION_REVISION
    PRODUCT_VERSION_REVISION_FROM_CI
    ${DEFAULT_PRODUCT_VERSION_REVISION}
    STRING
    "Version revision of the package to be built.")
endif()

# 用户在此处添加依赖包，添加后会更新输出各种变量供后续使用
# CONAN_PKGS: 本工程用到的所有的依赖包，conan install 时使用
# CONAN_PKG_OPTIONS: 本工程用到的所有依赖包的 options，conan install 时使用
# ${PACKAGE_BASE_NAME}_CONAN_PKG: 本依赖包的包名和版本，后续子项目使用
# ${PACKAGE_BASE_NAME}_CONAN_PKG_OPTIONS: 本依赖包的 options，后续子项目使用
# ${PACKAGE_BASE_NAME}_FIND_PACKAGE_NAME: 本依赖包的 find_package 名，后续子项目使用
# PLATFORM 参数: linux/windows
# add_conan_package("fmt/10.0.0" "fmt" "header_only=True") # 全平台依赖
add_conan_package("spdlog/1.11.0" "spdlog" "header_only=True")
add_conan_package("qt/5.15.14" "Qt5" "shared=True")
add_conan_package("doctest/2.4.11" "doctest" "")

# 运行 Conan install
include(conan_install)

include(conan_publisher_v1 OPTIONAL RESULT_VARIABLE publisher_include_result)
if(NOT publisher_include_result)
  message(WARNING "Failed to include cmake/conan_publisher_v1.cmake. Conan publish targets may not work properly.")
endif()

include(conan_publisher_macro)

# --- 定义 Conan 发布目标 ---
message(STATUS "Configuring Conan publish targets...")
# manage_subprojects.py 脚本会在此标记之间添加 define_conan_publish_targets_for_subproject() 调用
# CONAN_PUBLISH_TARGETS_BEGIN (do not remove or modify this line)
# 示例: define_conan_publish_targets_for_subproject("src/liba" "liba" "0.1.0" "" "fmt=fmt::fmt-header-only;spdlog=spdlog::spdlog_header_only" "libb")
# 参数1: 子项目路径 (相对于 CMakeLists.txt 的路径)
# 参数2: 包名 (Conan 包名)
# 参数3: 版本号 (Conan 包版本号)
# 参数4: 修订版本号 (Conan 包修订版本号)
# 参数5: 依赖的 Conan 包 (Conan 包依赖的其他包，格式为 "pkg1=target_name1=target_name2;find_name2=target_name2")
# 参数6: 依赖的本项目内部包 (Conan 包依赖的其他本项目内部包，多个包用分号分隔, 没有依赖的包可以留空字符串"")
# 版本号说明：
# 1. 使用本宏的版本号配合修订版本号变量作为版本号: 即此宏默认的方式，CI/CD 时修订版本为commit hash, 本地开发时修订版本为编译时间戳
# 2. tag 号作为版本: 若本项目只有一个工程，参数3设置为 "${PRODUCT_VERSION}" , 参数4设置为空""，然后通过仓库打tag的方式自动生成与tag号一致的版本号
# 3. 直接使用本宏设置版本号: 参数4设置为空字符串""
define_conan_publish_targets_for_subproject(
  "src/extsystem"
  "extsystem"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core;spdlog=spdlog::spdlog_header_only"
  "")
define_conan_publish_targets_for_subproject(
  "src/dscore"
  "dscore"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core=Qt5::Widgets=Qt5::LinguistTools;spdlog=spdlog::spdlog_header_only"
  "extsystem")
define_conan_publish_targets_for_subproject(
  "src/app"
  "app"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core=Qt5::Widgets;spdlog=spdlog::spdlog_header_only"
  "extsystem")
define_conan_publish_targets_for_subproject(
  "src/ws1"
  "ws1"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core=Qt5::Widgets=Qt5::LinguistTools;spdlog=spdlog::spdlog_header_only"
  "extsystem;dscore")
define_conan_publish_targets_for_subproject(
  "src/ws2"
  "ws2"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core=Qt5::Widgets=Qt5::LinguistTools;spdlog=spdlog::spdlog_header_only"
  "extsystem;dscore")
define_conan_publish_targets_for_subproject(
  "src/tests"
  "tests"
  "0.1.0"
  "${PRODUCT_VERSION_REVISION}"
  "qt=Qt5::Core=Qt5::Widgets=Qt5::Test;spdlog=spdlog::spdlog_header_only;doctest=doctest::doctest"
  "extsystem;dscore")
# CONAN_PUBLISH_TARGETS_END (do not remove or modify this line)
message(STATUS "Finished configuring Conan publish targets.")

# --- 添加子项目 ---
message(STATUS "Configuring subprojects...")
# manage_subprojects.py 脚本会在此标记之间添加 add_subdirectory() 调用
# SUBPROJECTS_BEGIN (do not remove or modify this line)
# 示例: add_subdirectory(src/liba)
add_subdirectory(src/extsystem)
add_subdirectory(src/dscore)
add_subdirectory(src/app)
add_subdirectory(src/ws1)
add_subdirectory(src/ws2)

add_subdirectory(src/tests)
# SUBPROJECTS_END (do not remove or modify this line)
message(STATUS "Finished configuring subprojects.")

# --- 创建用于发布所有配置包的顶层目标 ---
if(ALL_PUBLISH_TARGETS)
  add_custom_target(publish_all_conan_packages COMMENT "Build, package, and upload all configured Conan packages.")
  # 将所有单独的发布目标添加为此顶层目标的依赖项
  foreach(sub_target IN LISTS ALL_PUBLISH_TARGETS)
    add_dependencies(publish_all_conan_packages ${sub_target})
  endforeach()
  message(STATUS "Created target 'publish_all_conan_packages'.")
else()
  message(STATUS "No individual publish targets defined for 'publish_all_conan_packages'.")
endif()

message(STATUS "Project '${PROJECT_NAME}' configuration completed.")
message(STATUS "Use 'cmake --list-presets' to view available presets (including configurePresets and buildPresets).")
message(STATUS "Use 'cmake --preset <preset_name>' to configure the project.")
message(
  STATUS
    "Use 'cmake --build --preset <preset_name>' (if using presets) or 'cmake --build <binary_derectory> --target <target_name>' to build."
)
message(STATUS "Example: cmake --build --preset publish_windows-vs2022-config_release")
message(STATUS "Or: cmake --build build/windows-vs2022-config --target liba")
