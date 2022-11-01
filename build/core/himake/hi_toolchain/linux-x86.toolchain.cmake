cmake_minimum_required(VERSION 3.14)

if(LINUX_X86_TOOLCHAIN_INCLUDED)
  return()
endif(LINUX_X86_TOOLCHAIN_INCLUDED)
set(LINUX_X86_TOOLCHAIN_INCLUDED true)

# self-defined cache variable
# set(HI_SYSTEM_NAME HI_LINUX)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86)

# set(LINUX_X86_TOOLCHAIN_PREFIX x86_64-pc-linux-gnu)

# set(PREBUILT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../prebuilts)
# get_filename_component(PREBUILT_DIR ${PREBUILT_DIR} ABSOLUTE)

# set(CCACHE_PATH ${PREBUILT_DIR}/misc/linux-x86/ccache)
# set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PATH}/ccache)
# set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PATH}/ccache)

# set(GCC_VERSION 6.3.0)
# set(GCC_PATH ${PREBUILT_DIR}/gcc/linux-x86/x86/x86_64-pc-linux-gnu-${GCC_VERSION})
# set(CMAKE_C_COMPILER ${GCC_PATH}/bin/${LINUX_X86_TOOLCHAIN_PREFIX}-gcc)
# set(CMAKE_CXX_COMPILER ${GCC_PATH}/bin/${LINUX_X86_TOOLCHAIN_PREFIX}-g++)
# set(CMAKE_AR ${GCC_PATH}/bin/${LINUX_X86_TOOLCHAIN_PREFIX}-gcc-ar)
# set(CMAKE_RANLIB ${GCC_PATH}/bin/${LINUX_X86_TOOLCHAIN_PREFIX}-gcc-ranlib)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

set(GCC_VERSION 7.5.0)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_AR gcc-ar)
set(CMAKE_RANLIB gcc-ranlib)

set(LINUX TRUE)

if(NOT CMAKE_FIND_ROOT_PATH_MODE_PROGRAM)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
endif()

if(NOT CMAKE_FIND_ROOT_PATH_MODE_LIBRARY)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
endif()

if(NOT CMAKE_FIND_ROOT_PATH_MODE_INCLUDE)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

if(NOT CMAKE_FIND_ROOT_PATH_MODE_PACKAGE)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
endif()

set(LINUX_COMPILER_FLAGS)
list(APPEND LINUX_COMPILER_FLAGS
    -Wall -fPIC -no-pie -pthread
    -Wa,--noexecstack
    -ffunction-sections -fdata-sections -fvisibility=hidden
    -fno-common -Wextra -Wdate-time -Wfloat-equal -Wshadow -Wformat=2
    -fno-asynchronous-unwind-tables -fno-exceptions
    -fomit-frame-pointer -fno-unwind-tables)

if(HI_ABI MATCHES "m64")
    list(APPEND LINUX_COMPILER_FLAGS -m64)
elseif(ABI MATCHES "m32")
    list(APPEND LINUX_COMPILER_FLAGS -m32)
else()
    list(APPEND LINUX_COMPILER_FLAGS -m64)
endif()

list(APPEND LINUX_COMPILER_FLAGS
    -DHAVE_PTHREAD
    -D_FORTIFY_SOURCE=2)

set(LINUX_COMPILER_FLAGS_CXX)
list(APPEND LINUX_COMPILER_FLAGS_CXX
    -fno-rtti
    )

set(LINUX_COMPILER_FLAGS_DEBUG)
list(APPEND LINUX_COMPILER_FLAGS_DEBUG
    -g
    -O0)

set(LINUX_COMPILER_FLAGS_RELEASE)
list(APPEND LINUX_COMPILER_FLAGS_RELEASE
    -Os)

set(LINUX_LINKER_FLAGS)
list(APPEND LINUX_LINKER_FLAGS
    -Wl,-z,noexecstack
    -Wl,--no-undefined)

set(LINUX_EXE_LINKER_FLAGS)
list(APPEND LINUX_EXE_LINKER_FLAGS
    -Wl,--gc-sections)

string(REPLACE ";" " " LINUX_COMPILER_FLAGS "${LINUX_COMPILER_FLAGS}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_CXX "${LINUX_COMPILER_FLAGS_CXX}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_DEBUG "${LINUX_COMPILER_FLAGS_DEBUG}")
string(REPLACE ";" " " LINUX_COMPILER_FLAGS_RELEASE "${LINUX_COMPILER_FLAGS_RELEASE}")
string(REPLACE ";" " " LINUX_LINKER_FLAGS "${LINUX_LINKER_FLAGS}")
string(REPLACE ";" " " LINUX_EXE_LINKER_FLAGS "${LINUX_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS ""
    CACHE STRING "Flags used by the compiler during all build types.")
set(CMAKE_CXX_FLAGS ""
    CACHE STRING "Flags used by the compiler during all build types.")
# set(CMAKE_ASM_FLAGS ""
    # CACHE STRING "Flags used by the compiler during all build types.")
set(CMAKE_C_FLAGS_DEBUG ""
    CACHE STRING "Flags used by the compiler during debug builds.")
set(CMAKE_CXX_FLAGS_DEBUG ""
    CACHE STRING "Flags used by the compiler during debug builds.")
# set(CMAKE_ASM_FLAGS_DEBUG ""
    # CACHE STRING "Flags used by the compiler during debug builds.")
set(CMAKE_C_FLAGS_RELEASE ""
    CACHE STRING "Flags used by the compiler during release builds.")
set(CMAKE_CXX_FLAGS_RELEASE ""
    CACHE STRING "Flags used by the compiler during release builds.")
# set(CMAKE_ASM_FLAGS_RELEASE ""
    # CACHE STRING "Flags used by the compiler during release builds.")
set(CMAKE_MODULE_LINKER_FLAGS ""
    CACHE STRING "Flags used by the linker during the creation of modules.")
set(CMAKE_SHARED_LINKER_FLAGS ""
    CACHE STRING "Flags used by the linker during the creation of dll's.")
set(CMAKE_EXE_LINKER_FLAGS ""
    CACHE STRING "Flags used by the linker.")

set(CMAKE_C_FLAGS             "${LINUX_COMPILER_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS           "${LINUX_COMPILER_FLAGS} ${LINUX_COMPILER_FLAGS_CXX} ${CMAKE_CXX_FLAGS}")
# set(CMAKE_ASM_FLAGS           "${ANDROID_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG       "${LINUX_COMPILER_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG     "${LINUX_COMPILER_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}")
# set(CMAKE_ASM_FLAGS_DEBUG     "${ANDROID_COMPILER_FLAGS_DEBUG} ${CMAKE_ASM_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE     "${LINUX_COMPILER_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE   "${LINUX_COMPILER_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}")
# set(CMAKE_ASM_FLAGS_RELEASE   "${ANDROID_COMPILER_FLAGS_RELEASE} ${CMAKE_ASM_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${LINUX_LINKER_FLAGS} ${LINUX_LINKER_FLAGS_EXE} ${CMAKE_EXE_LINKER_FLAGS}")
