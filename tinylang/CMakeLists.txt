cmake_minimum_required(VERSION 3.4.3)
project(Tinylang)

# modified from: 
#   https://github.com/PacktPublishing/Learn-LLVM-12/blob/master/Chapter02/tinylang/CMakeLists.txt

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "In-source builds are not allowed. ")
endif()

set(TINYLANG_BUILT_STANDALONE 1)
set(TINYLANG_VERSION_STRING "0.1")

find_package(LLVM REQUIRED HINTS "${LLVM_CMAKE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(AddLLVM)           # use by ... 
include(HandleLLVMOptions) # /usr/local/lib/cmake/llvm/HandleLLVMOptions.cmake

# print
message(STATUS "========================[status]=======================")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "-------------------------------------------------------")
message(STATUS "LLVM_DIR = ${LLVM_DIR}")
message(STATUS "LLVM_CMAKE_PATH = ${LLVM_CMAKE_PATH}")
message(STATUS "LLVM_BINARY_DIR = ${LLVM_BINARY_DIR}")
message(STATUS "LLVM_INCLUDE_DIR = ${LLVM_INCLUDE_DIR}")
message(STATUS "LLVM_LIBRARY_DIR = ${LLVM_LIBRARY_DIR}")
message(STATUS "========================[status]=======================")


link_directories("${LLVM_LIBRARY_DIR}")
include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# cmake will create version.inc in the build directory corresponding to version.inc.in
# details and example at https://cmake.org/cmake/help/latest/command/configure_file.html#example
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tinylang/basic/version.inc.in # input
  ${CMAKE_CURRENT_BINARY_DIR}/include/tinylang/basic/version.inc    # output
)

include(AddTinylang)
add_subdirectory(lib)
add_subdirectory(tools)
