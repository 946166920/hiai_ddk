include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_glob)
include(hi_utils/hi_message) 

function(hi_cc_benchmark)
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
  )

  cmake_parse_arguments(HI_CC_BENCHMARK "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_BENCHMARK_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_benchmark(): \"${HI_CC_BENCHMARK_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_BENCHMARK_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_benchmark() function")
    return()
  endif()

  string(REPLACE :: _ name ${HI_CC_BENCHMARK_NAME})

  add_executable(${name})

  if(HI_CC_BENCHMARK_SRCS)
    hi_glob(srcs
      INCLUDES
        ${HI_CC_BENCHMARK_SRCS}
      EXCLUDES
        ${HI_CC_BENCHMARK_EXCLUDES}
    )
    target_sources(${name} PRIVATE ${srcs})
  endif()

  if(HI_CC_BENCHMARK_INCS)
    # reltive paths
    set_property(TARGET ${target_name} 
      PROPERTY HI_INCS ${HI_CC_BENCHMARK_INCS}
    )

    foreach(inc IN LISTS HI_CC_BENCHMARK_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    # absolute paths
    target_include_directories(${target_name}
      PRIVATE
        ${target_incs}
    )
  endif()

  if(HI_CC_BENCHMARK_DEPS)
    target_link_libraries(${name}
      PRIVATE 
        ${HI_CC_BENCHMARK_DEPS}
    )
  endif()

  target_link_libraries(${name}
    PRIVATE 
      benchmark_main
      benchmark
  )

  if(HI_CC_BENCHMARK_DEFS)
    target_compile_definitions(${name}
      PRIVATE 
        ${HI_CC_BENCHMARK_DEFS}
    )
  endif()

  if(HI_CC_BENCHMARK_COPTS)
    target_compile_options(${name}
      PRIVATE 
        ${HI_CC_BENCHMARK_COPTS}
    )
  endif()

  if(HI_CC_BENCHMARK_LOPTS)
    target_link_options(${name}
      PRIVATE 
        ${HI_CC_BENCHMARK_LOPTS}
    )
  endif()

  if(HI_CC_BENCHMARK_LPATHS)
    target_link_directories(${name}
      PRIVATE 
        ${HI_CC_BENCHMARK_LPATHS}
    )
  endif()

  add_test(NAME ${name} COMMAND ${name})
endfunction()