cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc-15)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++-15)
set(CMAKE_AR /usr/bin/aarch64-linux-gnu-gcc-ar-15)
set(CMAKE_NM /usr/bin/aarch64-linux-gnu-gcc-nm-15)
set(CMAKE_RANLIB /usr/bin/aarch64-linux-gnu-gcc-ranlib-15)
set(CMAKE_STRIP /usr/bin/aarch64-linux-gnu-strip)
set(CMAKE_OBJCOPY /usr/bin/aarch64-linux-gnu-objcopy)
set(CMAKE_OBJDUMP /usr/bin/aarch64-linux-gnu-objdump)

set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-a72")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-a72")
