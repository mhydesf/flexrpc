FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update &&                           \
    apt-get install -y --no-install-recommends  \
        ca-certificates                         \
        git                                     \
        openssh-client                          \
        # C/C++ Host Build System (x86 preset & CMake)
        build-essential                         \
        gcc-15                                  \
        g++-15                                  \
        libstdc++-15-dev                        \
        cmake                                   \
        ninja-build                             \
        pkg-config                              \
        gdb                                     \
        # AArch64 cross-toolchain, target libraries, debugger & QEMU user-mode emulation
        gcc-15-aarch64-linux-gnu                \
        g++-15-aarch64-linux-gnu                \
        binutils-aarch64-linux-gnu              \
        libc6-dev-arm64-cross                   \
        libstdc++-15-dev-arm64-cross            \
        libgcc-15-dev-arm64-cross               \
        gdb-multiarch                           \
        qemu-user                               \
        qemu-user-binfmt                        \
    && printf 'set debuginfod enabled off\nset auto-load safe-path /\nset sysroot /usr/aarch64-linux-gnu\n' > /etc/gdb/gdbinit.d/aarch64-cross.gdb \
    && rm -rf /var/lib/apt/lists/*
