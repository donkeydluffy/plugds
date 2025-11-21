# cmake/conan_publisher_macro.cmake

# 定义用于创建 Conan 发布目标的宏
# _SUBPROJECT_PATH_REL: 子项目相对根目录的路径 (如 "src/liba")
# _SUBPROJECT_CONAN_PKG_NAME: Conan 包名 (如 "liba")
# _PRODUCT_VERSION: 包版本 (如 "1.0.0")
# _VERSION_REVISION: 包修订版本 (如 "0" 或 "20231001")
# _DEPENDENT_PACKAGES: 依赖包的待链接的targets
# _DEPENDENT_SUBPROJECTS: 依赖的子项目列表 (如 "pj1;pj2")
macro(
  define_conan_publish_targets_for_subproject
  _SUBPROJECT_PATH_REL
  _SUBPROJECT_CONAN_PKG_NAME
  _PRODUCT_VERSION
  _PRODUCT_VERSION_REVISION
  _DEPENDENT_PACKAGES
  _DEPENDENT_SUBPROJECTS)
  message(
    STATUS "Defining Conan publish targets for subproject: ${_SUBPROJECT_CONAN_PKG_NAME} at ${_SUBPROJECT_PATH_REL}")
  message(
    STATUS
      "Product version: ${_PRODUCT_VERSION}, revision: ${_PRODUCT_VERSION_REVISION}, dependent packages: ${_DEPENDENT_PACKAGES}, dependent subprojects: ${_DEPENDENT_SUBPROJECTS}"
  )
  string(TOUPPER ${_SUBPROJECT_CONAN_PKG_NAME} project_name_upper)
  # 调整版本信息
  if("${_PRODUCT_VERSION}" STREQUAL "")
    message(
      WARNING
        "Package version not defined or empty, using default version 0.0.1 for package ${_SUBPROJECT_CONAN_PKG_NAME}")
    set(${project_name_upper}_PRODUCT_VERSION "0.0.1")
  else()
    set(${project_name_upper}_PRODUCT_VERSION "${_PRODUCT_VERSION}")
  endif()

  set(${project_name_upper}_PRODUCT_VERSION_REVISION "${_PRODUCT_VERSION_REVISION}")

  if(NOT DEFINED ${project_name_upper}_PRODUCT_VERSION_REVISION OR "${${project_name_upper}_PRODUCT_VERSION_REVISION}"
                                                                   STREQUAL "")
    # 在CI/CD 的 tag 下编译时，不使用修订版本
    set(${project_name_upper}_CONAN_PRODUCT_VERSION ${${project_name_upper}_PRODUCT_VERSION})
    message(
      STATUS
        "Package version revision not defined or empty, using package version ${${project_name_upper}_CONAN_PRODUCT_VERSION} for package ${_SUBPROJECT_CONAN_PKG_NAME}"
    )
  else()
    # 在 CI/CD 的非 tag下编译，或者在本地编译时，使用修订版本
    set(${project_name_upper}_CONAN_PRODUCT_VERSION
        "${${project_name_upper}_PRODUCT_VERSION}+${${project_name_upper}_PRODUCT_VERSION_REVISION}")
    message(
      STATUS
        "Package version revision defined, using package version ${${project_name_upper}_CONAN_PRODUCT_VERSION} for package ${_SUBPROJECT_CONAN_PKG_NAME}"
    )
  endif()

  # 定义支持的平台列表
  set(ALL_SUPPORTED_PLATFORM "WINDOWS;LINUX;MACOS;ANDROID")

  foreach(dep ${_DEPENDENT_PACKAGES}) # 遍历依赖的 Conan 包
    string(
      REPLACE "="
              ";"
              pkg_and_targets
              "${dep}")
    # 先得到依赖包的名字
    list(
      GET
      pkg_and_targets
      0
      depend_pkg_name)
    string(TOUPPER ${depend_pkg_name} depend_pkg_name_upper)

    # 处理全平台通用依赖
    if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG)
      list(APPEND ${project_name_upper}_CONAN_PKGS "${${depend_pkg_name_upper}_CONAN_PKG}")
      message(
        STATUS
          "Adding cross-platform dependency '${depend_pkg_name} : ${${depend_pkg_name_upper}_CONAN_PKG}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
      )
      message(STATUS "Updated ${project_name_upper}_CONAN_PKGS: ${${project_name_upper}_CONAN_PKGS}")
    endif()

    # 处理平台特定依赖
    foreach(platform IN LISTS ALL_SUPPORTED_PLATFORM) # 遍历所有支持的平台
      string(TOUPPER ${platform} platform_upper)
      if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG_${platform_upper})
        list(APPEND ${project_name_upper}_CONAN_PKGS_${platform_upper}
             "${${depend_pkg_name_upper}_CONAN_PKG_${platform_upper}}")
        message(
          STATUS
            "Adding platform-specific (${platform_upper}) dependency '${depend_pkg_name} : ${${depend_pkg_name_upper}_CONAN_PKG_${platform_upper}}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
        )
        message(
          STATUS
            "Updated ${project_name_upper}_CONAN_PKGS_${platform_upper}: ${${project_name_upper}_CONAN_PKGS_${platform_upper}}"
        )
      endif()
    endforeach()

    # 添加依赖包的通用选项
    if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG_OPTIONS)
      list(APPEND ${project_name_upper}_CONAN_PKG_OPTIONS "${${depend_pkg_name_upper}_CONAN_PKG_OPTIONS}")
      message(
        STATUS
          "Adding cross-platform dependency options '${${depend_pkg_name_upper}_CONAN_PKG_OPTIONS}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
      )
      message(STATUS "Updated ${project_name_upper}_CONAN_PKG_OPTIONS: ${${project_name_upper}_CONAN_PKG_OPTIONS}")
    endif()

    # 添加平台特定的依赖选项
    foreach(platform IN LISTS ALL_SUPPORTED_PLATFORM) # 遍历所有支持的平台
      string(TOUPPER ${platform} platform_upper)
      if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG_OPTIONS_${platform_upper})
        list(APPEND ${project_name_upper}_CONAN_PKG_OPTIONS_${platform_upper}
             "${${depend_pkg_name_upper}_CONAN_PKG_OPTIONS_${platform_upper}}")
        message(
          STATUS
            "Adding platform-specific (${platform_upper}) dependency options '${${depend_pkg_name_upper}_CONAN_PKG_OPTIONS_${platform_upper}}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
        )
        message(
          STATUS
            "Updated ${project_name_upper}_CONAN_PKG_OPTIONS_${platform_upper}: ${${project_name_upper}_CONAN_PKG_OPTIONS_${platform_upper}}"
        )
      endif()
    endforeach()

    # 添加依赖包的通用 find_package 名称
    if(DEFINED ${depend_pkg_name_upper}_FIND_PACKAGE_NAME)
      list(APPEND ${project_name_upper}_FIND_PACKAGE_NAMES "${${depend_pkg_name_upper}_FIND_PACKAGE_NAME}")
      message(
        STATUS
          "Adding cross-platform dependency find_package name '${${depend_pkg_name_upper}_FIND_PACKAGE_NAME}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
      )
      message(STATUS "Updated ${project_name_upper}_FIND_PACKAGE_NAMES: ${${project_name_upper}_FIND_PACKAGE_NAMES}")
    endif()

    # 添加平台特定的 find_package 名称
    foreach(platform IN LISTS ALL_SUPPORTED_PLATFORM) # 遍历所有支持的平台
      string(TOUPPER ${platform} platform_upper)
      if(DEFINED ${depend_pkg_name_upper}_FIND_PACKAGE_NAME_${platform_upper})
        list(APPEND ${project_name_upper}_FIND_PACKAGE_NAMES_${platform_upper}
             "${${depend_pkg_name_upper}_FIND_PACKAGE_NAME_${platform_upper}}")
        message(
          STATUS
            "Adding platform-specific (${platform_upper}) dependency find_package name '${${depend_pkg_name_upper}_FIND_PACKAGE_NAME_${platform_upper}}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
        )
        message(
          STATUS
            "Updated ${project_name_upper}_FIND_PACKAGE_NAMES_${platform_upper}: ${${project_name_upper}_FIND_PACKAGE_NAMES_${platform_upper}}"
        )
      endif()
    endforeach()

    # 剩下的部分是目标名称
    list(
      SUBLIST
      pkg_and_targets
      1
      -1
      targets_to_link)

    # 处理通用目标链接
    foreach(target_to_link IN LISTS targets_to_link)
      string(STRIP ${target_to_link} target_to_link_stripped)
      if(NOT
         target_to_link_stripped
         STREQUAL
         "")

        # 添加依赖包的通用 find_package 名称
        if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG)
          list(APPEND ${project_name_upper}_TARGETS_TO_LINK ${target_to_link_stripped})
          message(
            STATUS
              "Adding cross-platform dependency target '${target_to_link_stripped}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
          )
          message(STATUS "Updated ${project_name_upper}_TARGETS_TO_LINK: ${${project_name_upper}_TARGETS_TO_LINK}")
        endif()

        # 添加平台特定的 find_package 名称
        foreach(platform IN LISTS ALL_SUPPORTED_PLATFORM) # 遍历所有支持的平台
          string(TOUPPER ${platform} platform_upper)
          if(DEFINED ${depend_pkg_name_upper}_CONAN_PKG_${platform_upper})
            list(APPEND ${project_name_upper}_TARGETS_TO_LINK_${platform_upper} "${target_to_link_stripped}")
            message(
              STATUS
                "Adding platform-specific (${platform_upper}) dependency target '${target_to_link_stripped}' for package ${_SUBPROJECT_CONAN_PKG_NAME}"
            )
            message(
              STATUS
                "Updated ${project_name_upper}_TARGETS_TO_LINK_${platform_upper}: ${${project_name_upper}_TARGETS_TO_LINK_${platform_upper}}"
            )
          endif()
        endforeach()

      endif()
    endforeach()
  endforeach()

  foreach(dep ${_DEPENDENT_SUBPROJECTS}) # 遍历依赖子项目
    string(TOUPPER ${dep} dep_upper)
    list(APPEND ${project_name_upper}_CONAN_PKGS
         ${dep}/${${dep_upper}_CONAN_PRODUCT_VERSION}@${CONAN_USER_FOR_CMAKE}/${CONAN_CHANNEL_FOR_CMAKE})
    message(
      STATUS
        "Adding dependency '${dep} : ${dep}/${${dep_upper}_CONAN_PRODUCT_VERSION}@${CONAN_USER_FOR_CMAKE}/${CONAN_CHANNEL_FOR_CMAKE}' for package ${_SUBPROJECT_CONAN_PKG_NAME} (add subproject dependency)"
    )
    message(
      STATUS
        "Updated ${project_name_upper}_CONAN_PKGS: ${${project_name_upper}_CONAN_PKGS} (adding subproject dependency)")

    list(APPEND ${project_name_upper}_FIND_SUBPROJECT_PACKAGE_NAMES ${dep})
    message(
      STATUS
        "Adding dependency find_package for subproject name '${dep}' for package ${_SUBPROJECT_CONAN_PKG_NAME} (add subproject dependency)"
    )
    message(
      STATUS
        "Updated ${project_name_upper}_FIND_SUBPROJECT_PACKAGE_NAMES: ${${project_name_upper}_FIND_SUBPROJECT_PACKAGE_NAMES} (adding subproject dependency)"
    )

    list(APPEND ${project_name_upper}_TARGETS_TO_LINK ${dep}::${dep})
    message(
      STATUS
        "Adding dependency target '${dep}::${dep}' for package ${_SUBPROJECT_CONAN_PKG_NAME} (subproject dependency)")
    message(
      STATUS
        "Updated ${project_name_upper}_TARGETS_TO_LINK: ${${project_name_upper}_TARGETS_TO_LINK} (adding subproject dependency)"
    )
  endforeach()

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_SUBPROJECT_PATH_REL}/conanfile.py.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/${_SUBPROJECT_PATH_REL}/conanfile.py @ONLY NEWLINE_STYLE UNIX)

  get_conan_profile_prefix(CONAN_HOST_PROFILE_FOR_CREATE_PREFIX CONAN_BUILD_PROFILE_FOR_CREATE_PREFIX)
  # 为 Debug, Release 构建类型创建目标
  foreach(BUILD_TYPE IN ITEMS Debug Release)
    string(TOLOWER ${BUILD_TYPE} build_type_lower)
    # 构建 CMake 目标名称，例如 publish_liba_debug
    set(TARGET_NAME "publish_${_SUBPROJECT_CONAN_PKG_NAME}_${build_type_lower}")

    if(${CONAN_HOST_PROFILE_FOR_CREATE_PREFIX} STREQUAL "default" OR ${CONAN_BUILD_PROFILE_FOR_CREATE_PREFIX} STREQUAL
                                                                     "default")
      set(CONAN_HOST_PROFILE_FOR_CREATE "default")
      set(CONAN_BUILD_PROFILE_FOR_CREATE "default")
    else()
      set(CONAN_HOST_PROFILE_FOR_CREATE ${CONAN_HOST_PROFILE_FOR_CREATE_PREFIX}_${build_type_lower})
      set(CONAN_BUILD_PROFILE_FOR_CREATE ${CONAN_BUILD_PROFILE_FOR_CREATE_PREFIX}_${build_type_lower})
    endif()

    # 检查子项目的 conanfile.py 是否存在
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_SUBPROJECT_PATH_REL}/conanfile.py")
      message(
        WARNING
          "Conan recipe file not found at '${CMAKE_CURRENT_SOURCE_DIR}/${_SUBPROJECT_PATH_REL}/conanfile.py' (for package ${_SUBPROJECT_CONAN_PKG_NAME}). Target ${TARGET_NAME} may fail to build."
      )
    endif()

    # 添加自定义目标以调用 publish_single_package.cmake 脚本
    add_custom_target(
      ${TARGET_NAME}
      COMMAND
        # 将所有必要的参数传递给 CMake -P 脚本
        ${CMAKE_COMMAND} "-DLIBRARY_PATH_ARG=${_SUBPROJECT_PATH_REL}" "-DLIBRARY_NAME_ARG=${_SUBPROJECT_CONAN_PKG_NAME}"
        "-DLIBRARY_CONAN_VERSION_ARG=${${project_name_upper}_CONAN_PRODUCT_VERSION}"
        "-DCONAN_USER_ARG=${CONAN_USER_FOR_CMAKE}" "-DCONAN_CHANNEL_ARG=${CONAN_CHANNEL_FOR_CMAKE}"
        "-DCONAN_REMOTE_ARG=${CONAN_REMOTE_FOR_CMAKE}" "-DCONAN_CMD_PATH_ARG=${CONAN_CMD_PATH_INTERNAL}"
        "-DTOP_SOURCE_DIR_ARG=${CMAKE_CURRENT_SOURCE_DIR}" "-DBUILD_TYPE_ARG=${BUILD_TYPE}"
        "-DCONAN_PROFILE_HOST_ARG=${CONAN_HOST_PROFILE_FOR_CREATE}"
        "-DCONAN_PROFILE_BUILD_ARG=${CONAN_BUILD_PROFILE_FOR_CREATE}" "-DCONAN_USER_HOME_ARG=$ENV{CONAN_USER_HOME}"
        "-DTARGET_OS_NAME=${CMAKE_SYSTEM_NAME}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/publish_single_package.cmake"
      COMMENT "Build, package, and upload ${_SUBPROJECT_CONAN_PKG_NAME} (${BUILD_TYPE}) via Conan"
      VERBATIM # 确保命令按原样传递
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(dep ${_DEPENDENT_SUBPROJECTS})
      # 为每个依赖子项目添加依赖关系
      set(SUB_DEPEND_TARGET_NAME "publish_${dep}_${build_type_lower}")
      if(NOT DEFINED SUB_DEPEND_TARGET_NAME)
        message(
          FATAL_ERROR
            "Dependent subproject '${dep}' does not have a defined CONAN_PRODUCT_VERSION. Please ensure it is set correctly."
        )
      endif()
      add_dependencies(${TARGET_NAME} ${SUB_DEPEND_TARGET_NAME})
    endforeach()
    # 将此目标添加到 ALL_PUBLISH_TARGETS 列表
    list(APPEND ALL_PUBLISH_TARGETS ${TARGET_NAME})
    message(STATUS "Created Conan publish target for ${_SUBPROJECT_CONAN_PKG_NAME} (${BUILD_TYPE}): ${TARGET_NAME}")
  endforeach()
endmacro()
