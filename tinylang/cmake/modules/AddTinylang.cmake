macro(add_tinylang_subdirectory name) # like function
  message(STATUS ">>> add_tinylang_subdirectory = ${name}") # 2\ add_tinylang_subdirectory = driver
  message(STATUS ">>>    ARGN = ${ARGN}")
  add_llvm_subdirectory(TINYLANG TOOL ${name})
  # function(add_llvm_subdirectory project type name)
  # defined in llvm-project/llvm/cmake/modules/AddLLVM.cmake
endmacro()

macro(add_tinylang_library name)
  message(STATUS ">>> add_tinylang_library = ${name}") # 1\ add_tinylang_library = tinylangBasic
  message(STATUS ">>>    ARGN = ${ARGN}") # ARGN = version.cpp
  if(BUILD_SHARED_LIBS)
    set(LIBTYPE SHARED)
  else()
    set(LIBTYPE STATIC)
  endif()
  # function(llvm_add_library name)
  llvm_add_library(${name} ${LIBTYPE} ${ARGN})
  if(TARGET ${name})
    target_link_libraries(${name} INTERFACE ${LLVM_COMMON_LIBS})
    install(TARGETS ${name}
      COMPONENT ${name}
      LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
      ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
      RUNTIME DESTINATION bin)
  else()
    add_custom_target(${name})
  endif()
endmacro()

macro(add_tinylang_executable name)
  message(STATUS ">>> add_tinylang_executable = ${name}") # 4\ add_tinylang_executable = tinylang
  message(STATUS ">>>    ARGN = ${ARGN}") # ARGN = driver.cpp
  add_llvm_executable(${name} ${ARGN} )
endmacro()

macro(add_tinylang_tool name)
  message(STATUS ">>> add_tinylang_tool = ${name}") # 3\ add_tinylang_tool = tinylang
  message(STATUS ">>>    ARGN = ${ARGN}") # ARGN = driver.cpp
  add_tinylang_executable(${name} ${ARGN})
  install(TARGETS ${name}
    RUNTIME DESTINATION bin
    COMPONENT ${name})
endmacro()
