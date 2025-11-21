# 用于 Android 目标设备 (ARMv8)
[settings]
os=Android
arch=armv8
build_type=Release
os.api_level=24
compiler=clang
compiler.version=14
compiler.libcxx=c++_shared

[tool_requires]
android-ndk/r25b


# [conf]
# # 强制使用特定 NDK 路径（避免自动检测问题）
# tools.android:ndk_path=/root/.conan/data/android-ndk/r25b/_/_/package/4db1be536558d833e52e862fd84d64d75c2b3656/bin/

# [env]
# CHOST=aarch64-linux-android
# CC=/root/.conan/data/android-ndk/r25b/_/_/package/4db1be536558d833e52e862fd84d64d75c2b3656/bin/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang
# CXX=/root/.conan/data/android-ndk/r25b/_/_/package/4db1be536558d833e52e862fd84d64d75c2b3656/bin/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++
