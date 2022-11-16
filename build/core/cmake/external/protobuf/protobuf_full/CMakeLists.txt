### build outer dependency: protobuf-full

### define dir
set(protobuf_BUILD_TESTS OFF CACHE INTERNAL "not build protobuf test")

set(OPEN_SOURCE_ROOT_DIR ${ROOT_DIR}/third_party)
# set(PROTOBUF_OUT_DIR ${AI_BUILD_DIR}/protobuf)
set(PROTOBUF_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf_full)
set(PROTOBUF_INC_DIR ${OPEN_SOURCE_ROOT_DIR}/protobuf/src)


### call open source cmakes to build libprotobuf.so, etc.
set(BUILD_SHARED_LIBS TRUE)
set(protobuf_WITH_ZLIB  OFF CACHE INTERNAL "not build protobuf test")

add_subdirectory(${OPEN_SOURCE_ROOT_DIR}/protobuf/cmake ${PROTOBUF_OUT_DIR})

## libprotobuf
target_compile_definitions(libprotobuf
  PUBLIC
    GOOGLE_PROTOBUF_NO_RTTI
  PRIVATE
    HAVE_PTHREAD
    PROTOBUF_INLINE_NOT_IN_HEADERS=0
)

target_compile_options(libprotobuf
  PRIVATE
    -Wno-sign-compare -fvisibility=default
)

target_link_options(libprotobuf
  PRIVATE
    -static-libstdc++
)

if(CMAKE_SYSTEM_NAME MATCHES "Android")
  target_link_libraries(libprotobuf log)
endif()

add_library(google::protobuf_full_shared ALIAS libprotobuf)

## libprotobuf-lite
target_compile_definitions(libprotobuf-lite
  PUBLIC
    GOOGLE_PROTOBUF_NO_RTTI
  PRIVATE
    HAVE_PTHREAD
    PROTOBUF_INLINE_NOT_IN_HEADERS=0
)

if(CMAKE_SYSTEM_NAME MATCHES "Android")
  target_link_libraries(libprotobuf-lite log)
endif()

## libprotobuf
target_compile_options(libprotoc
  PRIVATE
    -Wno-sign-compare
    -fvisibility=default
)

target_link_options(libprotoc
  PRIVATE
    -static-libstdc++
)

target_compile_options(protoc
  PRIVATE
    -Wno-sign-compare
)

unset(BUILD_SHARED_LIBS)
