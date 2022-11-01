set(ANDROID_NATIVE_API_LEVEL "android-16")

if(HI_ABI MATCHES "arm64-v8a")
  set(ANDROID_ABI
      ${HI_ABI}
      CACHE STRING INTERNAL)
elseif(HI_ABI MATCHES "armeabi-v7a")
  set(ANDROID_ABI
      ${HI_ABI}
      CACHE STRING INTERNAL)
else()
  set(ANDROID_ABI
      "arm64-v8a"
      CACHE STRING INTERNAL)
endif()

set(HI_SYSTEM_NAME HI_ANDROID_NDK)

# include android ndk toolchain
set(NDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../../third_party/android-ndk-r20b)

list(APPEND CMAKE_MODULE_PATH
	${NDK_PATH}/build/cmake)
include(android.toolchain)

# default compile options Android.toolchain.cmake define these options as
# default: -ffunction-sections -fdata-sections -ffunction-sections
# -funwind-tables -fstack-protector-strong -no-canonical-prefixes
add_compile_options(
  -Wall
  -fPIC
  -no-pie
  -pthread
  -Os
  -fvisibility=hidden
  -fno-asynchronous-unwind-tables
  -fno-exceptions
  -fno-rtti
  -fomit-frame-pointer
  -fno-unwind-tables
  -Wextra
  -Wdate-time
  -Wfloat-equal
  -Wshadow
  -Wformat=2
  -fno-common)

# default link options Android.toolchain.cmake define these options as default:
# -Wl,--exclude-libs,libgcc.a -Wl,--exclude-libs,libatomic.a -Wl,--build-id
# -Wl,--warn-shared-textrel -Wl,--fatal-warnings -Wl,--exclude-libs,libunwind.a
set(CMAKE_SHARED_LINKER_FLAGS
    "-Wl,--no-undefined -Wl,--gc-sections -Wl,-s -Wl,-z,noexecstack -Wl,--exclude-libs,libgcc_real.a"
)
