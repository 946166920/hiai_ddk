include(ExternalProject)

set (mockcpp_CXXFLAGS "-fPIC -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -D_FORTIFY_SOURCE=2 -O2")
set (mockcpp_CFLAGS "-fPIC -Wno-dev")
set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/build CACHE STRING "path for install()" FORCE)

set(THIRD_PARTY_PATH ${TOP_DIR}/third_party)
set(MOCKCPP_DIR ${THIRD_PARTY_PATH}/mockcpp-2.7)

ExternalProject_Add(mockcpp_build
                    SOURCE_DIR ${MOCKCPP_DIR}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_CXX_FLAGS=${mockcpp_CXXFLAGS} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/mockcpp <SOURCE_DIR>
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_SHARED_LIBS=ON
                    BUILD_COMMAND $(MAKE)
                    INSTALL_COMMAND $(MAKE) install
                    EXCLUDE_FROM_ALL TRUE
)

set(MOCKCPP_PKG_DIR ${CMAKE_INSTALL_PREFIX}/mockcpp)

file(MAKE_DIRECTORY ${MOCKCPP_PKG_DIR}/include)

add_library(mockcpp STATIC IMPORTED)

set_target_properties(mockcpp PROPERTIES
    IMPORTED_LOCATION ${MOCKCPP_PKG_DIR}/lib/libmockcpp.a
)

target_include_directories(mockcpp INTERFACE ${MOCKCPP_PKG_DIR}/include)

set(INSTALL_BASE_DIR "")
set(INSTALL_LIBRARY_DIR lib)

install(FILES ${MOCKCPP_PKG_DIR}/lib/libmockcpp.a OPTIONAL
        DESTINATION ${INSTALL_LIBRARY_DIR})

add_dependencies(mockcpp mockcpp_build)

