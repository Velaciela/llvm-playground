#include "parser.h"

AST *Parser::parse() {
    AST *Res = parseCalc();
    expect(Token::eoi);
    return Res;
}

// calc : ("with" ident ("," ident)* ":")? expr ;
AST *Parser::parseCalc() {
    Expr *e;
    llvm::SmallVector<llvm::StringRef, 8> vars;

    // whether the optional group must be parsed or not
    // the group begins with the with token,
    // so we compare the token to this value:
    if (tok_.is(Token::KW_with)) {
        advance();

        if (expect(Token::ident))
            goto _error;
        vars.push_back(tok_.getText());
        advance();

        // separated by a comma
        //     The repetition group begins with the , token
        //     The test for the token becomes the condition of the while loop
        //     [?] The identifier inside the loop is treated as it was previously
        while (tok_.is(Token::comma)) {
            advance();
            if (expect(Token::ident))
                goto _error;
            vars.push_back(tok_.getText());
            advance();
        }

        if (consume(Token::colon))
            goto _error;
    }

    e = parseExpr();
    if (vars.empty()) 
        return e;
    else 
        return new WithDecl(vars, e);

// Detecting a syntax error is easy but recovering from it is surprisingly complicated
// a simple approach called panic mode must be used
_error:
    while (!tok_is(Token::eoi))
        advance();
    return nullptr;
}

