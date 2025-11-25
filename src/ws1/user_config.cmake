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

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/dscore/include")

# 设置用户自定义编译定义，根据需要添加
# list(APPEND PROJECT_COMPILE_DEFINITIONS MY_DEFINE=1 MY_FLAG)

# target_link_libraries(${PROJECT_NAME} PRIVATE dscore::dscore)
