# cmake/conan_env_setup.cmake

# --------- 定位 Conan 命令行工具 ---------
find_program(
  CONAN_CMD_PATH_INTERNAL
  NAMES conan conan.exe
  PATHS "${CMAKE_SOURCE_DIR}/.venv/bin" "${CMAKE_SOURCE_DIR}/.venv/Scripts" "${PATH}")
message(STATUS "Prepare using Conan: ${CONAN_CMD_PATH_INTERNAL}")

# --------- 设置缓存变量和环境变量等 ---------
include(utils)

define_new_variable(
  CONAN_USER_FOR_CMAKE
  CONAN_USER_FROM_CI
  "sss"
  STRING
  "Conan user ('_', 'sss')")

define_new_variable(
  CONAN_CHANNEL_FOR_CMAKE
  CONAN_CHANNEL_FROM_CI
  "testing"
  STRING
  "Conan channel ('_', 'stable', 'testing')")

define_new_variable(
  CONAN_REMOTE_FOR_CMAKE
  CONAN_REMOTE_FROM_CI
  "conan-local"
  STRING
  "Conan remote name")

define_new_variable(
  CONAN_REMOTE_URL_FOR_CMAKE
  CONAN_REMOTE_URL_FROM_CI
  "http://192.168.21.117:8081/artifactory/api/conan/conan-local"
  STRING
  "Conan remote URL")

define_new_variable(
  CONAN_LOGIN_USERNAME_FOR_CMAKE
  CONAN_LOGIN_USERNAME_FROM_CI
  ""
  STRING
  "Conan login username")

define_new_variable(
  CONAN_LOGIN_PASSWORD_FOR_CMAKE
  CONAN_LOGIN_PASSWORD_FROM_CI
  ""
  STRING
  "Conan login password")

# CONAN_USER_HOME 如果未定义或为空，则设置为默认用户目录
if(NOT DEFINED ENV{CONAN_USER_HOME} OR "$ENV{CONAN_USER_HOME}" STREQUAL "")
  if(WIN32)
    set(ENV{CONAN_USER_HOME} "$ENV{USERPROFILE}")
  else()
    set(ENV{CONAN_USER_HOME} "$ENV{HOME}")
  endif()
  message(STATUS "CONAN_USER_HOME not set, using: $ENV{CONAN_USER_HOME}")
else()
  message(STATUS "CONAN_USER_HOME already set: $ENV{CONAN_USER_HOME}")
endif()

# --------- 拷贝 Conan profiles ---------
set(PROFILE_COPY_FROM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/conan_profiles")
set(PROFILE_COPY_TO_DIR "$ENV{CONAN_USER_HOME}/.conan/profiles")
if(NOT EXISTS "${PROFILE_COPY_FROM_DIR}")
  message(WARNING "Conan profiles source directory '${PROFILE_COPY_FROM_DIR}' does not exist, skipping copy.")
  return()
endif()

file(MAKE_DIRECTORY "${PROFILE_COPY_TO_DIR}")
message(STATUS "Copying Conan profiles from '${PROFILE_COPY_FROM_DIR}' to '${PROFILE_COPY_TO_DIR}'")

file(
  GLOB PREDEFINED_PROFILE_FILES
  LIST_DIRECTORIES false
  RELATIVE "${PROFILE_COPY_FROM_DIR}"
  "${PROFILE_COPY_FROM_DIR}/*")

foreach(PREDEFINED_PROFILE_FILENAME IN LISTS PREDEFINED_PROFILE_FILES)
  get_filename_component(PROFILE_BASENAME "${PREDEFINED_PROFILE_FILENAME}" NAME_WE)
  set(SRC_PROFILE "${PROFILE_COPY_FROM_DIR}/${PREDEFINED_PROFILE_FILENAME}")
  set(DEST_PROFILE "${PROFILE_COPY_TO_DIR}/${PROFILE_BASENAME}")

  if(NOT EXISTS "${DEST_PROFILE}")
    message(STATUS "Copying profile '${PROFILE_BASENAME}'")

    # 拷贝文件
    file(COPY "${SRC_PROFILE}" DESTINATION "${PROFILE_COPY_TO_DIR}")

    # 重命名为不带后缀的名称
    if("${PREDEFINED_PROFILE_FILENAME}" MATCHES "\\.profile$")
      file(RENAME "${PROFILE_COPY_TO_DIR}/${PREDEFINED_PROFILE_FILENAME}" "${DEST_PROFILE}")
      message(STATUS "  Renamed to: '${PROFILE_BASENAME}'")
    elseif(
      NOT
      "${PREDEFINED_PROFILE_FILENAME}"
      STREQUAL
      "${PROFILE_BASENAME}")
      file(RENAME "${PROFILE_COPY_TO_DIR}/${PREDEFINED_PROFILE_FILENAME}" "${DEST_PROFILE}")
    endif()
  else()
    message(STATUS "Profile '${PROFILE_BASENAME}' already exists, skipping copy.")
  endif()
endforeach()

# --------- Conan 1.x 必须配置 ---------
if(NOT _CMAKE_ALREADY_RAN_CONAN_CONFIG_SET)
  message(STATUS "Setting Conan general.revisions_enabled=True ...")
  execute_process(
    COMMAND ${CONAN_CMD_PATH_INTERNAL} config set general.revisions_enabled=True
    RESULT_VARIABLE _conan_config_set_result
    OUTPUT_VARIABLE _conan_config_set_output
    ERROR_VARIABLE _conan_config_set_error)
  if(NOT
     _conan_config_set_result
     EQUAL
     0)
    message(
      WARNING
        "Executing 'conan config set general.revisions_enabled=True' may not have completed successfully (this is sometimes normal if the setting already exists):\nOutput:\n${_conan_config_set_output}\nError:\n${_conan_config_set_error}"
    )
  else()
    message(STATUS "Conan 'general.revisions_enabled=True' set successfully (or was already set).")
  endif()
  message(STATUS "Setting Conan general.use_always_short_paths=True ...")
  execute_process(
    COMMAND ${CONAN_CMD_PATH_INTERNAL} config set general.use_always_short_paths=True
    RESULT_VARIABLE _conan_config1_set_result
    OUTPUT_VARIABLE _conan_config1_set_output
    ERROR_VARIABLE _conan_config1_set_error)
  if(NOT
     _conan_config1_set_result
     EQUAL
     0)
    message(
      WARNING
        "Executing 'conan config set general.use_always_short_paths=True' may not have completed successfully (this is sometimes normal if the setting already exists):\nOutput:\n${_conan_config1_set_output}\nError:\n${_conan_config1_set_error}"
    )
  else()
    message(STATUS "Conan 'general.use_always_short_paths=True' set successfully (or was already set).")
  endif()
  set(_CMAKE_ALREADY_RAN_CONAN_CONFIG_SET
      TRUE
      CACHE INTERNAL "Prevent repeated setting of Conan config")
endif()

# --------- 自动检查并添加/登录 Conan Remote ---------
# set(CMAKE_ALREADY_RAN_CONAN_REMOTE_SETUP
#     TRUE
#     CACHE INTERNAL "Prevent repeated configuration of Conan remote and login")
if(NOT CMAKE_ALREADY_RAN_CONAN_REMOTE_SETUP)
  message(STATUS "Checking if Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' exists...")
  execute_process(
    COMMAND ${CONAN_CMD_PATH_INTERNAL} remote list
    OUTPUT_VARIABLE _conan_remote_list_output
    RESULT_VARIABLE _conan_remote_list_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(_remote_is_configured FALSE)
  if(_conan_remote_list_result EQUAL 0)
    string(
      REGEX MATCH
            "(^|\\n)${CONAN_REMOTE_FOR_CMAKE}:[ \t]+${CONAN_REMOTE_URL_FOR_CMAKE}"
            _remote_and_url_found
            "${_conan_remote_list_output}")
    string(
      REGEX MATCH
            "(^|\\n)${CONAN_REMOTE_FOR_CMAKE}:"
            _remote_name_found
            "${_conan_remote_list_output}")

    if(_remote_and_url_found)
      message(STATUS "Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' already exists and its URL matches.")
      set(_remote_is_configured TRUE)
    elseif(_remote_name_found)
      message(
        STATUS
          "Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' exists but the URL may not match. Will try to update it using --force."
      )
      # 将继续执行 remote add --force
    else()
      message(STATUS "Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' not found. Attempting to add it...")
    endif()

    # 如果未配置，或者已配置但我们想强制更新URL
    if(NOT _remote_is_configured OR _remote_name_found)
      if("${CONAN_REMOTE_URL_FOR_CMAKE}" STREQUAL "")
        message(
          WARNING
            "CONAN_REMOTE_URL_FOR_CMAKE not set, cannot automatically add/update remote repository '${CONAN_REMOTE_FOR_CMAKE}'."
        )
      else()
        message(
          STATUS
            "Adding/updating remote repository '${CONAN_REMOTE_FOR_CMAKE}' with URL: ${CONAN_REMOTE_URL_FOR_CMAKE} ...")
        execute_process(
          COMMAND ${CONAN_CMD_PATH_INTERNAL} remote add ${CONAN_REMOTE_FOR_CMAKE} ${CONAN_REMOTE_URL_FOR_CMAKE} --force
                  -i 0
          RESULT_VARIABLE _conan_remote_add_result
          OUTPUT_VARIABLE _conan_remote_add_output
          ERROR_VARIABLE _conan_remote_add_error)
        if(NOT
           _conan_remote_add_result
           EQUAL
           0)
          message(
            WARNING
              "Failed to add/update Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}':\nOutput:\n${_conan_remote_add_output}\nError:\n${_conan_remote_add_error}"
          )
        else()
          message(
            STATUS
              "Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' added/updated successfully.\n${_conan_remote_add_output}"
          )
          # 标记为已配置（或至少尝试配置了）
          set(_remote_is_configured TRUE)
        endif()
      endif()
    endif()
  else()
    message(WARNING "Failed to execute 'conan remote list', unable to check remote repository.")
  endif()

  # 只有在远程仓库已配置（或尝试配置）后才尝试登录
  if(_remote_is_configured)
    # 先检查是否已经登录
    execute_process(
      COMMAND ${CONAN_CMD_PATH_INTERNAL} user -r ${CONAN_REMOTE_FOR_CMAKE}
      RESULT_VARIABLE _check_login_result
      OUTPUT_VARIABLE _check_login_output
      ERROR_VARIABLE _check_login_error
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT
       _check_login_result
       EQUAL
       0)
      message(FATAL_ERROR "Failed to execute 'conan user' to check login status:\n${_check_login_error}")
    endif()

    string(FIND "${_check_login_output}" "Authenticated" _auth_found)
    if(${_auth_found} EQUAL -1)
      if(CONAN_LOGIN_PASSWORD_FOR_CMAKE STREQUAL "")
        message(
          STATUS
            "Conan login password (CONAN_LOGIN_PASSWORD_FOR_CMAKE) is empty, attempting login without a password (if permitted by the remote or if credentials are cached)."
        )
      endif()

      if(NOT
         "${CONAN_LOGIN_USERNAME_FOR_CMAKE}"
         STREQUAL
         "")
        message(
          STATUS
            "Attempting to log in to Conan remote repository '${CONAN_REMOTE_FOR_CMAKE}' with user: '${CONAN_LOGIN_USERNAME_FOR_CMAKE}' ..."
        )
        set(CONAN_LOGIN_ARGS user)
        if(NOT
           "${CONAN_LOGIN_PASSWORD_FOR_CMAKE}"
           STREQUAL
           "")
          list(
            APPEND
            CONAN_LOGIN_ARGS
            -p
            "${CONAN_LOGIN_PASSWORD_FOR_CMAKE}")
        endif()
        list(
          APPEND
          CONAN_LOGIN_ARGS
          "${CONAN_LOGIN_USERNAME_FOR_CMAKE}"
          -r
          ${CONAN_REMOTE_FOR_CMAKE})

        execute_process(
          COMMAND ${CONAN_CMD_PATH_INTERNAL} ${CONAN_LOGIN_ARGS}
          RESULT_VARIABLE _conan_login_result
          OUTPUT_VARIABLE _conan_login_output
          ERROR_VARIABLE _conan_login_error)
        if(NOT
           _conan_login_result
           EQUAL
           0)
          message(
            WARNING
              "Conan login to remote repository '${CONAN_REMOTE_FOR_CMAKE}' (user: '${CONAN_LOGIN_USERNAME_FOR_CMAKE}') may have failed (this might be normal if already logged in or if the remote does not require a password):\nOutput:\n${_conan_login_output}\nError:\n${_conan_login_error}"
          )
        else()
          message(
            STATUS
              "Conan login to remote repository '${CONAN_REMOTE_FOR_CMAKE}' (user: '${CONAN_LOGIN_USERNAME_FOR_CMAKE}') succeeded.\n${_conan_login_output}"
          )
        endif()
      else()
        message(
          FATAL_ERROR
            "CONAN_LOGIN_USERNAME_FOR_CMAKE is not set. Please set CONAN_LOGIN_USERNAME_FOR_CMAKE and CONAN_LOGIN_PASSWORD_FOR_CMAKE."
        )
      endif()
    else()
      message(
        STATUS
          "Already logged in to Conan remote '${CONAN_REMOTE_FOR_CMAKE}' (user: '${CONAN_LOGIN_USERNAME_FOR_CMAKE}')")
    endif()

  endif()

  set(CMAKE_ALREADY_RAN_CONAN_REMOTE_SETUP
      TRUE
      CACHE INTERNAL "Prevent repeated configuration of Conan remote and login")
endif()
