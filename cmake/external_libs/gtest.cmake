include(ExternalProject)

set (gtest_CXXFLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 -D_FORTIFY_SOURCE=2 -O2")
set (gtest_CFLAGS "-D_FORTIFY_SOURCE=2 -O2")
set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/build CACHE STRING "path for install()" FORCE)

set(THIRD_PARTY_PATH ${TOP_DIR}/third_party)
set (GTEST_DIR ${THIRD_PARTY_PATH}/googletest-release-1.8.1)

ExternalProject_Add(gtest_build
                    SOURCE_DIR ${GTEST_DIR} 
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_CXX_FLAGS=${gtest_CXXFLAGS} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/gtest <SOURCE_DIR>
                    -DBUILD_TESTING=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_SHARED_LIBS=ON
                    BUILD_COMMAND $(MAKE)
                    INSTALL_COMMAND $(MAKE) install
                    EXCLUDE_FROM_ALL TRUE
)

set(GTEST_PKG_DIR ${CMAKE_INSTALL_PREFIX}/gtest)

file(MAKE_DIRECTORY ${GTEST_PKG_DIR}/include)

add_library(gtest SHARED IMPORTED)

set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgtest.so
)

add_library(gtest_main SHARED IMPORTED)

set_target_properties(gtest_main PROPERTIES
    IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgtest_main.so
)

target_include_directories(gtest INTERFACE ${GTEST_PKG_DIR}/include)
target_include_directories(gtest_main INTERFACE ${GTEST_PKG_DIR}/include)

set(INSTALL_BASE_DIR "")
set(INSTALL_LIBRARY_DIR lib)

install(FILES ${GTEST_PKG_DIR}/lib/libgtest.so ${GTEST_PKG_DIR}/lib/libgtest_main.so OPTIONAL
        DESTINATION ${INSTALL_LIBRARY_DIR})

add_dependencies(gtest gtest_build)

set(HAVE_GTEST TRUE)
