include_guard()

include(CMakePrintHelpers)
include(hi_make/hi_system)
include(hi_utils/hi_message)

function(hi_cc_test)
  if(NOT BUILD_TESTING)
    return()
  endif()

  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    INCS
    SRCS
    EXCLUDES
    DEPS
    CDEFS
    COPTS
    LOPTS
    LPATHS
    WHOLE_STATIC_LIBS
    GROUP_STATIC_LIBS
  )

  cmake_parse_arguments(HI_CC_TEST "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_TEST_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_test(): \"${HI_CC_TEST_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_TEST_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_test() function")
    return()
  endif()

  string(REPLACE :: _ name ${HI_CC_TEST_NAME})

  add_executable(${name})

  if(HI_CC_TEST_SRCS)
    hi_glob(srcs
      INCLUDES
        ${HI_CC_TEST_SRCS}
      EXCLUDES
        ${HI_CC_TEST_EXCLUDES}
    )
    target_sources(${target_name} PRIVATE ${srcs})
  endif()

  if(HI_CC_TEST_INCS)
    # reltive paths
    set_property(TARGET ${target_name} 
      PROPERTY HI_INCS ${HI_CC_TEST_INCS}
    )

    foreach(inc IN LISTS HI_CC_TEST_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    # absolute paths
    target_include_directories(${target_name}
      PRIVATE
        ${target_incs}
    )
  endif()

  if(HI_CC_TEST_DEPS)
    target_link_libraries(${name}
      PRIVATE 
        ${HI_CC_TEST_DEPS}
    )
  endif()

  target_link_libraries(${name}
    PRIVATE 
      gtest_main
      gtest
      gcov
  )

  if(HI_CC_TEST_DEFS)
    target_compile_definitions(${name}
      PRIVATE 
        ${HI_CC_TEST_DEFS}
    )
  endif()

  if(HI_CC_TEST_COPTS)
    target_compile_options(${name}
      PRIVATE 
        ${HI_CC_TEST_COPTS}
    )
  endif()

  if(HI_CC_TEST_LOPTS)
    target_link_options(${name}
      PRIVATE 
        ${HI_CC_TEST_LOPTS}
    )
  endif()

  if(HI_CC_TEST_LPATHS)
    target_link_directories(${name}
      PRIVATE 
        ${HI_CC_TEST_LPATHS}
    )
  endif()

  if(HI_CC_TEST_WHOLE_STATIC_LIBS)
    target_link_libraries(${target_name} 
      PRIVATE -Wl,--whole-archive ${HI_CC_TEST_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
    )
  endif()

  if(HI_CC_TEST_GROUP_STATIC_LIBS)
    target_link_libraries(${target_name} 
      -Wl,--start-group ${HI_CC_TEST_GROUP_STATIC_LIBS} -Wl,--end-group
    )
    
  endif()  

  add_test(NAME ${name} COMMAND ${name})
endfunction()