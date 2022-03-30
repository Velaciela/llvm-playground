// All the phases from the previous sections are glued together by the calc.cpp driver
// We delegated the object code generation to the LLVM static compiler, llc

#include "code_gen.h"
#include "parser.h"
#include "sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

// LLVM comes with its own system for declaring command-line options
static llvm::cl::opt<std::string> Input(
    llvm::cl::Positional, 
    llvm::cl::desc("<input expression>"),
    llvm::cl::init("")
);

int main(int argc, const char **argv) {
    llvm::InitLLVM x(argc, argv); // initialize LLVM lib
    llvm::cl::ParseCommandLineOptions(
        argc, argv, "calc - the expression compiler\n");

    Lexer lex(Input);
    Parser parser(Lex);
    AST *tree = Parser.parse();
    // check if errors occurred after syntactical analysis
    if (!tree || parser.hasError()) {
        llvm::errs() << "Syntax errors occured\n";
        return 1;
    }

    Sema semantic;
    if (semantic.semantic(tree)) {
        llvm::errs() << "Semantic errors occured\n";
        return 1;
    }
    
    CodeGen code_generator;
    code_generator.compile(tree);
    return 0;
}
