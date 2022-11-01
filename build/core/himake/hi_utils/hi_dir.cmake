include_guard()

include(hi_utils/hi_message)

function(hi_add_subdirectory dir)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${dir})
  else()
    hi_message(STATUS "WARNING: not found dir ${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
  endif()
endfunction()

function(hi_add_extdirectory srcdir outdir)
  if(EXISTS ${srcdir})
    add_subdirectory(${srcdir} ${outdir})
  else()
    hi_message(WARNING "WARNING: can't find dir ${srcdir}.")
  endif()
endfunction()