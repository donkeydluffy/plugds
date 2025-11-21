[settings]
os=Linux
arch=armv8
build_type=RelWithDebInfo
compiler=gcc
compiler.libcxx=libstdc++
compiler.version=9
[options]
# libx265/*:assembly=False
# ffmpeg/*:with_asm=False
# qt/*:with_mysql=False
# qt/*:cross_compile=aarch64-linux-gnu
[build_requires]
[env]
CC=aarch64-linux-gnu-gcc
CXX=aarch64-linux-gnu-g++
STRIP=aarch64-linux-gnu-strip
AR=aarch64-linux-gnu-ar
RANLIB=aarch64-linux-gnu-ranlib
AS=aarch64-linux-gnu-as
CMAKE_ASM_COMPILER=aarch64-linux-gnu-as
OBJCOPY=aarch64-linux-gnu-objcopy
# for ffmpeg build 
# [buildenv] # for ffmpeg build 
# CC=aarch64-linux-gnu-gcc
# CXX=aarch64-linux-gnu-g++
# STRIP=aarch64-linux-gnu-strip
# AR=aarch64-linux-gnu-ar
# RANLIB=aarch64-linux-gnu-ranlib
# AS=aarch64-linux-gnu-as
# CMAKE_ASM_COMPILER=aarch64-linux-gnu-as
[conf]
tools.build:compiler_executables={"c": "aarch64-linux-gnu-gcc", "cpp": "aarch64-linux-gnu-g++"}
