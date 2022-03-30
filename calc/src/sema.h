#pragma once

#include "ast.h"
#include "lexer.h"

class Sema {

public:
    bool semantic(AST *tree);

};
