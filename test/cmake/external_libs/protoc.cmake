include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/build CACHE STRING "path for install()" FORCE)
set(protobuf_CXXFLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 -fPIC -D_FORTIFY_SOURCE=2 -O2 -pthread -Wno-dev")
set(protobuf_LDFLAGS " -Wl,-z,relro,-z,now,-z,noexecstack -lpthread")

set(THIRD_PARTY_PATH ${TOP_DIR}/third_party)
set(PROTOBUF_DIR ${THIRD_PARTY_PATH}/protobuf/)

ExternalProject_Add(protoc_build
                    SOURCE_DIR ${PROTOBUF_DIR}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -Dprotobuf_WITH_ZLIB=OFF -Dprotobuf_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS=${protobuf_CXXFLAGS} -DCMAKE_CXX_LDFLAGS=${protobuf_LDFLAGS} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/protoc <SOURCE_DIR>/cmake
                    BUILD_COMMAND $(MAKE)
                    INSTALL_COMMAND $(MAKE) install
                    EXCLUDE_FROM_ALL TRUE
                    BUILD_IN_SOURCE  TRUE
)

set(PROTOC_PKG_DIR ${CMAKE_INSTALL_PREFIX}/protoc)

set(protoc_EXECUTABLE ${PROTOC_PKG_DIR}/${CMAKE_INSTALL_BINDIR}/protoc)

function(protobuf_generate comp c_var h_var)
    if(NOT ARGN)
        message(SEND_ERROR "Error: protobuf_generate() called without any proto files")
        return()
    endif()
    set(${c_var})
    set(${h_var} ${BASE_DIR}/build/proto/${comp})

    foreach(file ${ARGN})
        get_filename_component(abs_file ${file} ABSOLUTE)
        get_filename_component(file_name ${file} NAME_WE)
        get_filename_component(file_dir ${abs_file} PATH)
        get_filename_component(parent_subdir ${file_dir} NAME)

        if("${parent_subdir}" STREQUAL "proto")
            set(proto_output_path ${BASE_DIR}/build/proto/${comp}/proto)
        else()
            set(proto_output_path ${BASE_DIR}/build/proto/${comp}/proto/${parent_subdir})
        endif()

        list(APPEND ${c_var} "${proto_output_path}/${file_name}.pb.cc")

        add_custom_command(
                OUTPUT "${proto_output_path}/${file_name}.pb.cc" "${proto_output_path}/${file_name}.pb.h"
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${proto_output_path}"
                COMMAND ${protoc_EXECUTABLE} -I${file_dir} --cpp_out=${proto_output_path} ${abs_file}
                DEPENDS protoc_build
                COMMENT "Running C++ protocol buffer compiler on ${file}" VERBATIM )
    endforeach()

    set_source_files_properties(${${c_var}} {${h_var}} PROPERTIES GENERATED TRUE)
    set(${c_var} ${${c_var}} PARENT_SCOPE)
    set(${h_var} ${${h_var}} PARENT_SCOPE)

endfunction()