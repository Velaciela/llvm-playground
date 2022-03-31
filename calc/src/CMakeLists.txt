add_executable (calc
    lexer.cpp
    parser.cpp
    sema.cpp
    code_gen.cpp
    calc.cpp
)

target_link_libraries (calc 
    PRIVATE 
    ${llvm_libs}
)

install(TARGETS calc
    RUNTIME DESTINATION bin
    COMPONENT calc
)