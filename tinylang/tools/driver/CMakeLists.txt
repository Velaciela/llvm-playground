set(LLVM_LINK_COMPONENTS
  Support
)

# macro @ cmake/modules/AddTinylang.cmake
add_tinylang_tool(tinylang
  driver.cpp
)

# https://cmake.org/cmake/help/latest/command/target_link_libraries.html
target_link_libraries(tinylang # target
  PRIVATE                      # link dependencies
  tinylangBasic                # item
)
