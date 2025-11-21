include(conan)

set(CONAN_INSTALL_PKGS ${CONAN_PKGS})
set(CONAN_INSTALL_PKG_OPTIONS ${CONAN_PKG_OPTIONS})

# 定义支持的平台列表
set(ALL_PLATFORM "Windows;Linux;Darwin;Android")
# 处理平台特定依赖
foreach(platform IN LISTS ALL_PLATFORM) # 遍历所有支持的平台
  string(TOUPPER ${platform} platform_upper)
  if(DEFINED CONAN_PKGS_${platform_upper}
     AND DEFINED CMAKE_SYSTEM_NAME
     AND CMAKE_SYSTEM_NAME STREQUAL ${platform})
    list(APPEND CONAN_INSTALL_PKGS "${CONAN_PKGS_${platform_upper}}")
    message(STATUS "Adding platform-specific (${platform_upper}) packages '${CONAN_PKGS_${platform_upper}}' to install")

    list(APPEND CONAN_INSTALL_PKG_OPTIONS ${CONAN_PKG_OPTIONS_${platform_upper}})
    message(
      STATUS
        "Adding platform-specific (${platform_upper}) package options '${CONAN_PKG_OPTIONS_${platform_upper}}' to install"
    )
  endif()
endforeach()

if(DEFINED CONAN_INSTALL_PKGS
   AND NOT
       "${CONAN_INSTALL_PKGS}"
       STREQUAL
       "")
  message(STATUS "Conan packages to install: ${CONAN_INSTALL_PKGS}")

  find_program(
    CONAN_COMMAND
    NAMES conan conan.exe
    PATHS "${CMAKE_SOURCE_DIR}/.venv/bin" "${CMAKE_SOURCE_DIR}/.venv/Scripts" "${PATH}")
  message(STATUS "Install using Conan: ${CONAN_COMMAND}")

  get_conan_profile_prefix(CONAN_HOST_PROFILE_FOR_INSTALL_PREFIX CONAN_BUILD_PROFILE_FOR_INSTALL_PREFIX)

  if(DEFINED CMAKE_CONFIGURATION_TYPES
     AND NOT
         "${CMAKE_CONFIGURATION_TYPES}"
         STREQUAL
         "")
    foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
      if(TYPE STREQUAL "MinSizeRel" OR TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "Skipping Conan install for MinSizeRel AND RelWithDebInfo configuration.")
        continue()
      else()
        string(TOLOWER ${TYPE} build_type_lower)
        if(${CONAN_HOST_PROFILE_FOR_INSTALL_PREFIX} STREQUAL "default" OR ${CONAN_BUILD_PROFILE_FOR_INSTALL_PREFIX}
                                                                          STREQUAL "default")
          set(CONAN_HOST_PROFILE_FOR_INSTALL "default")
          set(CONAN_BUILD_PROFILE_FOR_INSTALL "default")
        else()
          set(CONAN_HOST_PROFILE_FOR_INSTALL "${CONAN_HOST_PROFILE_FOR_INSTALL_PREFIX}_${build_type_lower}")
          set(CONAN_BUILD_PROFILE_FOR_INSTALL "${CONAN_BUILD_PROFILE_FOR_INSTALL_PREFIX}_${build_type_lower}")
        endif()

        conan_cmake_configure(
          REQUIRES
          ${CONAN_INSTALL_PKGS}
          GENERATORS
          CMakeDeps
          CMakeToolchain
          OPTIONS
          ${CONAN_INSTALL_PKG_OPTIONS})

        conan_cmake_install(
          PATH_OR_REFERENCE
          .
          REMOTE
          ${CONAN_REMOTE_FOR_CMAKE}
          PROFILE_HOST
          ${CONAN_HOST_PROFILE_FOR_INSTALL}
          PROFILE_BUILD
          ${CONAN_BUILD_PROFILE_FOR_INSTALL})
      endif()
    endforeach()
  else()
    if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      message(FATAL_ERROR "Unsupported Conan install for MinSizeRel AND RelWithDebInfo configuration.")
    else()
      string(TOLOWER ${CMAKE_BUILD_TYPE} build_type_lower)

      if(${CONAN_HOST_PROFILE_FOR_INSTALL_PREFIX} STREQUAL "default" OR ${CONAN_BUILD_PROFILE_FOR_INSTALL_PREFIX}
                                                                        STREQUAL "default")
        set(CONAN_HOST_PROFILE_FOR_INSTALL "default")
        set(CONAN_BUILD_PROFILE_FOR_INSTALL "default")
      else()
        set(CONAN_HOST_PROFILE_FOR_INSTALL "${CONAN_HOST_PROFILE_FOR_INSTALL_PREFIX}_${build_type_lower}")
        set(CONAN_BUILD_PROFILE_FOR_INSTALL "${CONAN_BUILD_PROFILE_FOR_INSTALL_PREFIX}_${build_type_lower}")
      endif()
    endif()

    conan_cmake_configure(
      REQUIRES
      ${CONAN_INSTALL_PKGS}
      GENERATORS
      CMakeDeps
      CMakeToolchain
      OPTIONS
      ${CONAN_INSTALL_PKG_OPTIONS})

    conan_cmake_install(
      PATH_OR_REFERENCE
      .
      BUILD
      missing
      REMOTE
      ${CONAN_REMOTE_FOR_CMAKE}
      PROFILE_HOST
      ${CONAN_HOST_PROFILE_FOR_INSTALL}
      PROFILE_BUILD
      ${CONAN_BUILD_PROFILE_FOR_INSTALL})
  endif()
else()
  message(STATUS "No Conan packages needed. Skip install.")
endif()
