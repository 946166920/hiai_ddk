include_guard()

include(CMakeParseArguments)

include(hi_utils/hi_message)

function(hi_glob files)
  set(opts)

  set(single_args)

  set(multi_args
    INCLUDES
    EXCLUDES
  )

  cmake_parse_arguments(HI_GLOB "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  
  if(HI_GLOB_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_glob(): \"${HI_GLOB_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_GLOB_INCLUDES)
    hi_message(FATAL_ERROR "required keyword INCLUDES missing for hi_glob() function")
    return()
  endif()

  foreach(file IN LISTS HI_GLOB_INCLUDES)
    string(FIND ${file} "*" result)
    if (result EQUAL -1)  # non glob
      get_source_file_property(is_generated ${file} GENERATED)
      if(is_generated EQUAL 1)
        list(APPEND generated_files ${file})
      else()
        string(FIND ${file} "/" result)
        if(result EQUAL 0)
          if(NOT EXISTS ${file})
            hi_message(FATAL_ERROR "not found ${file}")
            return()
          endif()
          list(APPEND absolute_files ${file})
        else()
          if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
            hi_message(FATAL_ERROR "not found ${file} in ${CMAKE_CURRENT_SOURCE_DIR}")
            return()
          endif()
          list(APPEND relative_files ${file})
        endif()
      endif()
    else()   # glob
      string(FIND ${file} "/" result)
      if(result EQUAL 0)
        list(APPEND absolute_globs ${file})
      else()
        list(APPEND relative_globs ${file})
      endif()      
    endif()
  endforeach()

  if(absolute_globs)
    file(GLOB absolute_glob_files CONFIGURE_DEPENDS
      ${absolute_globs}
    )
    list(APPEND absolute_files ${absolute_glob_files})
  endif()

  if(relative_globs)
    file(GLOB relative_glob_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
      ${relative_globs}
    )
    list(APPEND relative_files ${relative_glob_files})
  endif()

  foreach(exclude_file IN LISTS HI_GLOB_EXCLUDES)
    if(IS_ABSOLUTE ${exclude_file})
      if(EXISTS ${exclude_file})
        list(REMOVE_ITEM absolute_files ${exclude_file})
      else()
        hi_message(FATAL_ERROR "EXCLUDES invalid: ${exclude_file} is not exist")
        return()
      endif()
    else()
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${exclude_file})
        list(REMOVE_ITEM relative_files ${exclude_file})
      else()
        hi_message(FATAL_ERROR "EXCLUDES invalid: ${CMAKE_CURRENT_SOURCE_DIR}/${exclude_file} is not exist")
        return()
      endif()
    endif()
  endforeach()

  list(APPEND all_files ${absolute_files} ${relative_files} ${generated_files})

  set(${files} ${all_files} PARENT_SCOPE)

  if(NOT all_files)
    hi_message(WARNING "not found any files in ${CMAKE_CURRENT_SOURCE_DIR} | includes: ${HI_GLOB_INCLUDES} | exculdes: ${HI_GLOB_EXCLUDES}")
  endif()
endfunction()