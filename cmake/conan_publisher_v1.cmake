# cmake/conan_publisher_v1.cmake 包含用于 Conan 1.x 的发布逻辑。
# 函数：为 Conan 1.x 执行 conan create 和 conan upload
# _conan_cmd_path: Conan 命令的路径 (例如 "/usr/local/bin/conan")
# _subproject_abs_path: 子项目的绝对路径 (包含 conanfile.py)
# _pkg_conan_name: 要创建的 Conan 包的名称
# _pkg_conan_version: Conan 包的版本
# _conan_user, _conan_channel, _conan_remote: Conan 上传参数
# _current_build_type: 当前的构建类型 (Debug, Release, RelWithDebInfo)
# _profile_host: 主机配置文件的名称 (例如 "conan1_gcc13_release")
# _profile_build: 构建配置文件的名称 (例如 "conan1_gcc13_release")
# _conan_user_home: Conan 用户主目录的路径 (例如 "/home/user/.conan")
# _target_os_name: 目标操作系统名称 (例如 "linux", "windows")
function(
  conan1_do_create_and_upload
  _conan_cmd_path
  _subproject_abs_path
  _pkg_conan_name
  _pkg_conan_version
  _conan_user
  _conan_channel
  _conan_remote
  _current_build_type
  _profile_host
  _profile_build
  _conan_user_home
  _target_os_name)

  message(STATUS "Conan 1.x publisher: Starting package ${_pkg_conan_name}/${_pkg_conan_version}")
  message(STATUS "  Subproject path: ${_subproject_abs_path}")
  message(STATUS "  Build type: ${_current_build_type}")

  string(TOUPPER ${_pkg_conan_name} pkg_conan_name_upper)
  # 设置环境变量，conan 运行时会使用这些变量
  set(ENV{CONAN_USER_HOME} "${_conan_user_home}")
  set(ENV{CONAN_TARGET_OS_NAME} "${_target_os_name}")
  # 下面这些是 conanfile.py 中使用
  set(ENV{${pkg_conan_name_upper}_CONAN_CREATE_SOURCE_PATH} "${_subproject_abs_path}")

  # 构建包引用
  set(PACKAGE_REFERENCE "${_pkg_conan_name}/${_pkg_conan_version}")
  if(NOT
     "${_conan_user}"
     STREQUAL
     ""
     AND NOT
         "${_conan_channel}"
         STREQUAL
         "")
    string(APPEND PACKAGE_REFERENCE "@${_conan_user}/${_conan_channel}")
  elseif(
    NOT
    "${_conan_user}"
    STREQUAL
    "")
    string(APPEND PACKAGE_REFERENCE "@${_conan_user}")
  endif()

  # 在 conan create 之前先清理本地缓存
  message(STATUS "  Cleaning local cache for package: ${PACKAGE_REFERENCE}")
  set(CONAN_REMOVE_ARGS remove ${PACKAGE_REFERENCE} --force)

  execute_process(
    COMMAND ${_conan_cmd_path} ${CONAN_REMOVE_ARGS}
    RESULT_VARIABLE result_remove
    OUTPUT_VARIABLE output_remove
    ERROR_VARIABLE error_remove)

  if(NOT
     result_remove
     EQUAL
     0)
    # conan remove 失败通常是因为包不存在，这是正常情况，不需要中断流程
    message(STATUS "  Conan remove completed (package may not exist in cache): ${output_remove}")
  else()
    message(STATUS "  Conan remove succeeded: ${output_remove}")
  endif()

  # 构建 Conan Create 命令参数列表 (Conan 1.x 语法)
  set(CONAN_CREATE_ARGS create "${_subproject_abs_path}")
  list(APPEND CONAN_CREATE_ARGS ${PACKAGE_REFERENCE})

  list(APPEND CONAN_CREATE_ARGS --profile:host=${_profile_host})
  list(APPEND CONAN_CREATE_ARGS --profile:build=${_profile_build})
  list(
    APPEND
    CONAN_CREATE_ARGS
    -s
    build_type=${_current_build_type})

  list(APPEND CONAN_CREATE_ARGS --build=missing)
  list(APPEND CONAN_CREATE_ARGS --update)

  message(STATUS "  Executing Conan 1.x Create: ${_conan_cmd_path} ${CONAN_CREATE_ARGS}")
  execute_process(
    COMMAND ${_conan_cmd_path} ${CONAN_CREATE_ARGS}
    RESULT_VARIABLE result_create
    OUTPUT_VARIABLE output_create
    ERROR_VARIABLE error_create)

  if(NOT
     result_create
     EQUAL
     0)
    message(
      FATAL_ERROR
        "Conan 1.x create failed (package: ${_pkg_conan_name}/${_pkg_conan_version}, build type: ${_current_build_type}):\nOutput:\n${output_create}\nError:\n${error_create}"
    )
  else()
    message(
      STATUS
        "Conan 1.x create succeeded (package: ${_pkg_conan_name}/${_pkg_conan_version}, build type: ${_current_build_type}):\n${output_create}"
    )
  endif()

  # Conan Upload 上传
  set(PACKAGE_REFERENCE_TO_UPLOAD ${PACKAGE_REFERENCE})

  if(NOT _conan_remote OR "${_conan_remote}" STREQUAL "")
    message(WARNING "Conan remote not specified. Skipping upload for ${PACKAGE_REFERENCE_TO_UPLOAD}.")
    return()
  endif()

  set(CONAN_UPLOAD_ARGS
      upload
      ${PACKAGE_REFERENCE_TO_UPLOAD}
      --all
      -r
      ${_conan_remote}
      --confirm
      --force)
  message(STATUS "  Executing Conan 1.x Upload: ${_conan_cmd_path} ${CONAN_UPLOAD_ARGS}")

  execute_process(
    COMMAND ${_conan_cmd_path} ${CONAN_UPLOAD_ARGS}
    RESULT_VARIABLE result_upload
    OUTPUT_VARIABLE output_upload
    ERROR_VARIABLE error_upload)

  if(NOT
     result_upload
     EQUAL
     0)
    message(
      WARNING
        "Conan 1.x upload failed (package: ${PACKAGE_REFERENCE_TO_UPLOAD} to remote ${_conan_remote}):\nOutput:\n${output_upload}\nError:\n${error_upload}"
    )
  else()
    message(
      STATUS
        "Conan 1.x upload succeeded (package: ${PACKAGE_REFERENCE_TO_UPLOAD} uploaded to remote ${_conan_remote}):\n${output_upload}"
    )
  endif()
  message(STATUS "Conan 1.x publisher: Finished processing package ${_pkg_conan_name}/${_pkg_conan_version}")
endfunction()
