include_guard()

function(hi_message)
  if(NOT WIN32)
    string(ASCII 27 esc)
    set(red          "${esc}[31m")
    set(green        "${esc}[32m")
    set(yellow       "${esc}[33m")
    set(blue         "${esc}[34m")
    set(magenta      "${esc}[35m")
    set(cyan         "${esc}[36m")
    set(white        "${esc}[37m")
    set(bold_red     "${esc}[1;31m")
    set(bold_green   "${esc}[1;32m")
    set(bold_yellow  "${esc}[1;33m")
    set(bold_blue    "${esc}[1;34m")
    set(bold_magenta "${esc}[1;35m")
    set(bold_cyan    "${esc}[1;36m")
    set(bold_white   "${esc}[1;37m")
    set(reset_color  "${esc}[m")
    set(bold_color   "${esc}[1m")    
  endif()

  list(GET ARGV 0 level)
  if(level STREQUAL FATAL_ERROR OR level STREQUAL SEND_ERROR)
    list(REMOVE_AT ARGV 0)
    message(${level} "${bold_red}${ARGV}${reset_color}")
  elseif(level STREQUAL WARNING)
    list(REMOVE_AT ARGV 0)
    message(${level} "${bold_yellow}${ARGV}${reset_color}")
  elseif(level STREQUAL AUTHOR_WARNING)
    list(REMOVE_AT ARGV 0)
    message(${level} "${bold_cyan}${ARGV}${reset_color}")
  elseif(level STREQUAL STATUS)
    list(REMOVE_AT ARGV 0)
    message(${level} "${green}${ARGV}${reset_color}")
  else()
    message("${ARGV}")
  endif()
endfunction()