include_guard()

include(hi_utils/hi_message)
include(hi_utils/hi_target)

function(hi_command)
  set(opts)

  set(single_args
    PY
    SH
    TARGET
    TIME
  )

  set(multi_args
    OPTS
  )

  cmake_parse_arguments(HI_COMMAND "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_COMMAND_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_command(): \"${HI_COMMAND_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_COMMAND_TARGET)
    hi_message(FATAL_ERROR "target not found for hi_command()")
    return()
  endif()

  if(HI_COMMAND_PY AND HI_COMMAND_SH)
    hi_message(FATAL_ERROR "both python and shell scripts found for hi_command()")
    return()
  endif()

  if(NOT HI_COMMAND_PY AND NOT HI_COMMAND_SH)
    hi_message(FATAL_ERROR "both python and shell scripts not found for hi_command()")
    return()
  endif()

  if(HI_COMMAND_PY)
    if(NOT EXISTS ${HI_COMMAND_PY})
      hi_message(FATAL_ERROR "not found ${HI_COMMAND_PY} for hi_command() function")
      return()
    endif()

    set(hi_exec_cmd ${Python_EXECUTABLE} -B ${HI_COMMAND_PY} ${HI_COMMAND_OPTS})
  endif()

  if(HI_COMMAND_SH)
    if(NOT EXISTS ${HI_COMMAND_SH})
      hi_message(FATAL_ERROR "not found ${HI_COMMAND_SH} for hi_command() function")
      return()
    endif()
    set(hi_exec_cmd ${HI_COMMAND_SH} ${HI_COMMAND_OPTS})
  endif()

  if(HI_COMMAND_TIME)
    set(hi_exec_time ${HI_COMMAND_TIME})
  else()
    set(hi_exec_time POST_BUILD)
  endif()

  if(HI_COMMAND_TARGET)
    hi_get_target_aliased_name(${HI_COMMAND_TARGET} target_name)
    add_custom_command(
      TARGET
        ${target_name}
      ${hi_exec_time}
      COMMAND
        ${hi_exec_cmd}
      VERBATIM
    )
  endif()
endfunction()
