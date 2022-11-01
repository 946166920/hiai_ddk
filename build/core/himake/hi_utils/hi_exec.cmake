include_guard()

include(hi_utils/hi_message)

function(hi_exec result)
  set(opts)

  set(single_args
    PY
    SH
  )

  set(multi_args
    OPTS
  )

  find_package(Python REQUIRED COMPONENTS Interpreter)

  cmake_parse_arguments(HI_EXEC "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_EXEC_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_exec(): \"${HI_EXEC_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(HI_EXEC_PY)
    if(NOT EXISTS ${HI_EXEC_PY})
      hi_message(FATAL_ERROR "not found ${HI_EXEC_PY} for hi_exec() function")
      return()
    endif()

    execute_process(
      COMMAND
        ${Python_EXECUTABLE} -B ${HI_EXEC_PY} ${HI_EXEC_OPTS}
      OUTPUT_VARIABLE
        out
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE  
        err_info
      RESULT_VARIABLE 
        err_no
    )  

    if(NOT ${err_no} EQUAL 0)
      hi_message(FATAL_ERROR "hi_exec run failed: ${HI_EXEC_PY}: ${err_no}, ${err_info}")
      return()
    endif()    
  endif()

  if(HI_EXEC_SH)
    if(NOT EXISTS ${HI_EXEC_SH})
      hi_message(FATAL_ERROR "not found ${HI_EXEC_SH} for hi_exec() function")
      return()
    endif()

    execute_process(
      COMMAND
        ${HI_EXEC_SH} ${HI_EXEC_OPTS}
      OUTPUT_VARIABLE
        out
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE
        err_info
      RESULT_VARIABLE
        err_no
    )

    if(NOT ${err_no} EQUAL 0)
      hi_message(FATAL_ERROR "hi_exec run failed: ${HI_EXEC_SH}: ${err_info}")
      return()
    endif()
  endif()

  set(${result} "${out}" PARENT_SCOPE)

endfunction()
