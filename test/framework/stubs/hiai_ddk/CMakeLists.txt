cmake_minimum_required(VERSION 3.8.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)
set(FRAMEWORK_BASE_DIR_FOR_INC_DIRS ${TOP_DIR}/src/framework)

include(ai_client.cmake)
include(hiai.cmake)
include(hiai_ir.cmake)
include(hiai_ir_build.cmake)
include(hiai_ir_build_aipp.cmake)






