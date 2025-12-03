cmake_minimum_required(VERSION 3.23)

project(
  ws2
  LANGUAGES CXX
  VERSION 0.1.0)

# find_package
set(PACKAGES_VAR "WS2_FIND_PACKAGE_NAMES")
if(NOT DEFINED ${PACKAGES_VAR})
  message(STATUS "No packages defined for project ${PROJECT_NAME}")
endif()

# 获取当前平台信息用于平台特定处理
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(CURRENT_PLATFORM "ANDROID")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CURRENT_PLATFORM "WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(CURRENT_PLATFORM "MACOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CURRENT_PLATFORM "LINUX")
else()
  set(CURRENT_PLATFORM "UNKNOWN")
endif()

if(CURRENT_PLATFORM AND DEFINED WS2_FIND_PACKAGE_NAMES_${CURRENT_PLATFORM})
  set(PACKAGE_VAR_PLATFORM "WS2_FIND_PACKAGE_NAMES_${CURRENT_PLATFORM}")
  list(APPEND WS2_FIND_PACKAGE_NAMES "${${PACKAGE_VAR_PLATFORM}}")
endif()

set(PACKAGE_LIST "${${PACKAGES_VAR}}")

if(NOT PACKAGE_LIST)
  message(STATUS "No packages to find for project ${PROJECT_NAME}")
endif()

message(STATUS "Finding packages for ${PROJECT_NAME}: ${PACKAGE_LIST}")

set(FOUND_PACKAGES "")
set(FAILED_PACKAGES "")

# 遍历包列表
foreach(PACKAGE_NAME ${PACKAGE_LIST})
  message(STATUS "  Finding ${PACKAGE_NAME}...")

  find_package(${PACKAGE_NAME} CONFIG REQUIRED)

  if(${PACKAGE_NAME}_FOUND)
    list(APPEND FOUND_PACKAGES ${PACKAGE_NAME})
    message(STATUS "  OK Found ${PACKAGE_NAME}")
  else()
    list(APPEND FAILED_PACKAGES ${PACKAGE_NAME})
    message(FATAL_ERROR "  ERROR Required package not found: ${PACKAGE_NAME}")
  endif()
endforeach()

if(FOUND_PACKAGES)
  message(STATUS "Successfully found packages for ${PROJECT_NAME}: ${FOUND_PACKAGES}")
endif()

# --- 项目类型和选项 ---
set(PROJECT_TYPE_CMAKE_VAR "lib")

option(WS2_BUILD_SHARED_LIBS "Build shared library (when project type is 'lib')" ON)

# 使用 CMake 的 if 语句处理 CMake 变量
if(PROJECT_TYPE_CMAKE_VAR STREQUAL "exe")
  set(WS2_BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "Executables cannot be built as shared libraries" FORCE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- 自动查找所有源文件和头文件 ---
file(
  GLOB_RECURSE
  PROJECT_SOURCES_GLOBBED
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
file(
  GLOB_RECURSE
  PROJECT_HEADERS_GLOBBED
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.inl")
# Explicitly set UI files (avoiding glob issues)
file(
  GLOB_RECURSE
  PROJECT_UIFILES_GLOBBED
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
file(
  GLOB_RECURSE
  PROJECT_QRCFILES_GLOBBED
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
file(
  GLOB_RECURSE
  PROJECT_TSFILES_GLOBBED
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ts")
qt5_add_translation(PROJECT_QM_FILES ${PROJECT_TSFILES_GLOBBED})

# Generate a qrc file for the translations
set(TRANSLATIONS_QRC_CONTENT "<RCC>\n    <qresource prefix=\"/ws2/i18n\">
")
foreach(QM_FILE ${PROJECT_QM_FILES})
  get_filename_component(QM_FILENAME ${QM_FILE} NAME)
  string(APPEND TRANSLATIONS_QRC_CONTENT "        <file alias=\"${QM_FILENAME}\">${QM_FILE}</file>\n")
endforeach()
string(APPEND TRANSLATIONS_QRC_CONTENT "    </qresource>\n</RCC>\n")

set(TRANSLATIONS_QRC_FILE "${CMAKE_CURRENT_BINARY_DIR}/ws2_translations.qrc")
file(WRITE ${TRANSLATIONS_QRC_FILE} "${TRANSLATIONS_QRC_CONTENT}")

qt5_add_resources(QMFILES_RES ${TRANSLATIONS_QRC_FILE})

file(
  GLOB_RECURSE
  PROJECT_PUBLIC_HEADERS
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.hxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.inl")

# 定义 API 导出/导入宏
if(WS2_BUILD_SHARED_LIBS)
  set(GENERATE_API_MACROS TRUE)
else()
  set(GENERATE_API_MACROS FALSE)
endif()

# 为库生成只包含头文件的方案
if(NOT PROJECT_HEADERS_GLOBBED)
  if(GENERATE_API_MACROS)
    message(
      STATUS
        "No headers found in ${CMAKE_CURRENT_SOURCE_DIR}. Generating default .h file for shared library '${PROJECT_NAME}'.")
    set(EXPORT_MACROS_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/export_macros.h")
    file(
      WRITE ${EXPORT_MACROS_HEADER_FILE}
      "#pragma once

#if defined(_WIN32)
#if defined(${PROJECT_NAME}_EXPORTS)
#define WS2_API __declspec(dllexport)
#else
#define WS2_API __declspec(dllimport)
#endif
#else  // Non-Windows
#define WS2_API __attribute__((visibility(\"default\")))
#endif
")
    list(APPEND PROJECT_HEADERS_GLOBBED ${EXPORT_MACROS_HEADER_FILE})
  endif()
endif()

# --- 版本号等信息 ---
set(WS2_PRODUCT_NAME ${PROJECT_NAME})
set(WS2_PRODUCT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(WS2_PRODUCT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(WS2_PRODUCT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(WS2_PRODUCT_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
if(WS2_PRODUCT_VERSION EQUAL 0
   OR (WS2_PRODUCT_VERSION
       AND NOT
           "${WS2_PRODUCT_VERSION}"
           STREQUAL
           ""
      ))
  set(WS2_PRODUCT_VERSION_MAJOR 0)
  set(WS2_PRODUCT_VERSION_MINOR 0)
  set(WS2_PRODUCT_VERSION_PATCH 0)
  set(WS2_PRODUCT_VERSION_TWEAK 0)
  string(
    REGEX MATCHALL
          "[0-9]+"
          VERSION_LIST
          ${WS2_PRODUCT_VERSION})
  list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)
  if(VERSION_LIST_LENGTH GREATER 0)
    list(
      GET
      VERSION_LIST
      0
      WS2_PRODUCT_VERSION_MAJOR)
  endif()
  if(VERSION_LIST_LENGTH GREATER 1)
    list(
      GET
      VERSION_LIST
      1
      WS2_PRODUCT_VERSION_MINOR)
  endif()
  if(VERSION_LIST_LENGTH GREATER 2)
    list(
      GET
      VERSION_LIST
      2
      WS2_PRODUCT_VERSION_PATCH)
  endif()
  if(VERSION_LIST_LENGTH GREATER 3)
    list(
      GET
      VERSION_LIST
      3
      WS2_PRODUCT_VERSION_TWEAK)
  endif()
endif()

if(NOT WS2_PRODUCT_VERSION_REVISION OR "${WS2_PRODUCT_VERSION_REVISION}" STREQUAL "")
  set(WS2_PRODUCT_FULL_VERSION
      "${WS2_PRODUCT_VERSION_MAJOR}.${WS2_PRODUCT_VERSION_MINOR}.${WS2_PRODUCT_VERSION_PATCH}.${WS2_PRODUCT_VERSION_TWEAK}"
  )
else()
  set(WS2_PRODUCT_FULL_VERSION
      "${WS2_PRODUCT_VERSION_MAJOR}.${WS2_PRODUCT_VERSION_MINOR}.${WS2_PRODUCT_VERSION_PATCH}+${WS2_PRODUCT_VERSION_REVISION}"
  )
endif()

set(WS2_PRODUCT_FILE_FULL_VERSION
    "${WS2_PRODUCT_VERSION_MAJOR}.${WS2_PRODUCT_VERSION_MINOR}.${WS2_PRODUCT_VERSION_PATCH}.${WS2_PRODUCT_VERSION_TWEAK}"
)

string(TIMESTAMP PRODUCT_CURRENT_YEAR "%Y")
set(WS2_PRODUCT_COMPANY_COPYRIGHT
    "Copyright (c) 2016-${PRODUCT_CURRENT_YEAR} SCANTECH (HANGZHOU) CO., LTD. All Rights Reserved.")

# --- 添加目标 ---
set(IS_INTERFACE_LIBRARY FALSE)
if(NOT PROJECT_SOURCES_GLOBBED AND PROJECT_TYPE_CMAKE_VAR STREQUAL "lib")
  message(STATUS "${PROJECT_NAME}: Building as INTERFACE library (no sources found or provided).")
  add_library(${PROJECT_NAME} INTERFACE)
  add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  set(IS_INTERFACE_LIBRARY TRUE)
elseif(PROJECT_TYPE_CMAKE_VAR STREQUAL "exe")
  message(STATUS "${PROJECT_NAME}: Building as executable.")
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_GLOBBED} ${PROJECT_HEADERS_GLOBBED})
  add_executable(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  set(_VersionResourceFile ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
  configure_file(${CMAKE_CURRENT_LIST_DIR}/version.rc.in ${_VersionResourceFile} @ONLY)
  target_sources(${PROJECT_NAME} PRIVATE ${_VersionResourceFile})
else()
  if(WS2_BUILD_SHARED_LIBS)
    message(STATUS "${PROJECT_NAME}: Building as shared library.")
    add_library(
      ${PROJECT_NAME} SHARED
      ${PROJECT_SOURCES_GLOBBED}
      ${PROJECT_HEADERS_GLOBBED}
      ${PROJECT_UIFILES_GLOBBED}
      ${PROJECT_QRCFILES_GLOBBED}
      ${QMFILES_RES})
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
    if(WIN32 AND GENERATE_API_MACROS)
      list(APPEND PROJECT_COMPILE_DEFINITIONS ${PROJECT_NAME}_EXPORTS)
      set(_VersionResourceFile ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
      configure_file(${CMAKE_CURRENT_LIST_DIR}/version.rc.in ${_VersionResourceFile} @ONLY)
      target_sources(${PROJECT_NAME} PRIVATE ${_VersionResourceFile})
    endif()
  else()
    message(STATUS "${PROJECT_NAME}: Building as static library.")
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES_GLOBBED} ${PROJECT_HEADERS_GLOBBED})
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  endif()
endif()

if(PROJECT_SOURCES_GLOBBED)
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PROJECT_SOURCES_GLOBBED})
endif()
if(PROJECT_HEADERS_GLOBBED AND PROJECT_TYPE_CMAKE_VAR STREQUAL "lib")
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PROJECT_HEADERS_GLOBBED})
endif()

if(PROJECT_PUBLIC_HEADERS)
  target_sources(
    ${PROJECT_NAME}
    PUBLIC FILE_SET
           HEADERS
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}/include"
           FILES
           ${PROJECT_PUBLIC_HEADERS})
endif()

if(IS_INTERFACE_LIBRARY)
  target_include_directories(${PROJECT_NAME} INTERFACE $<INSTALL_INTERFACE:include>)
  target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
else()
  target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                    $<INSTALL_INTERFACE:include>)
  target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
  target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
endif()

foreach(OUTPUT_CONFIG IN ITEMS RUNTIME LIBRARY)
  set_target_properties(${PROJECT_NAME} PROPERTIES ${OUTPUT_CONFIG}_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
endforeach()

# linking
set(PKGS_TO_LINK "WS2_TARGETS_TO_LINK")
if(CURRENT_PLATFORM AND DEFINED WS2_TARGETS_TO_LINK_${CURRENT_PLATFORM})
  set(PKGS_TO_LINK_PLATFROM "WS2_TARGETS_TO_LINK_${CURRENT_PLATFORM}")
  list(APPEND WS2_TARGETS_TO_LINK "${${PKGS_TO_LINK_PLATFROM}}")
endif()
foreach(PACKAGE_NAME ${${PKGS_TO_LINK}})
  if(IS_INTERFACE_LIBRARY)
    target_link_libraries(${PROJECT_NAME} INTERFACE ${PACKAGE_NAME})
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PACKAGE_NAME})
  endif()
  message(STATUS "Linking ${PACKAGE_NAME} to ${PROJECT_NAME}")
endforeach()

include(${CMAKE_CURRENT_SOURCE_DIR}/user_config.cmake)

if(DEFINED PROJECT_COMPILE_OPTIONS AND PROJECT_COMPILE_OPTIONS)
  if(IS_INTERFACE_LIBRARY)
    target_compile_options(${PROJECT_NAME} INTERFACE ${PROJECT_COMPILE_OPTIONS})
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
  endif()
endif()
if(NOT WIN32)
  if(IS_INTERFACE_LIBRARY)
    target_compile_options(${PROJECT_NAME} INTERFACE -fPIC)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
  endif()
endif()

if(DEFINED PROJECT_COMPILE_DEFINITIONS AND PROJECT_COMPILE_DEFINITIONS)
  if(IS_INTERFACE_LIBRARY)
    target_compile_definitions(${PROJECT_NAME} INTERFACE ${PROJECT_COMPILE_DEFINITIONS})
  else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
  endif()
endif()

# --- 安装规则 ---
if(PROJECT_HEADERS_GLOBBED)
  install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
            FILE_SET HEADERS
            DESTINATION include)
else()
  install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

if(MSVC
   AND NOT IS_INTERFACE_LIBRARY
   AND WS2_BUILD_SHARED_LIBS)
  target_compile_options(${PROJECT_NAME} PRIVATE $<$<AND:$<CONFIG:Release>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/Zi>)
  target_link_options(
    ${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Release>:/DEBUG>
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>)
  install(
    FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
    DESTINATION bin
    CONFIGURATIONS Debug Release RelWithDebInfo)
endif()

install(
  EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
if(NOT DEFINED CONAN_PACKAGE_VERSION_FOR_CMAKE)
  message(
    WARNING "CONAN_PACKAGE_VERSION_FOR_CMAKE is not defined for ${PROJECT_NAME}. Falling back to project version.")
  set(CONAN_PACKAGE_VERSION_FOR_CMAKE ${PROJECT_VERSION})
endif()

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${CONAN_PACKAGE_VERSION_FOR_CMAKE}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file("${PROJECT_NAME}Config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" DESTINATION lib/cmake/${PROJECT_NAME})

message(STATUS "CMakeLists.txt configured for ${PROJECT_NAME} (type: ${PROJECT_TYPE_CMAKE_VAR}).")
