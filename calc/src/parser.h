#pragma once

#include "ast.h"
#include "lexer.h"

#include "llvm/Support/raw_ostream.h"
// The coding guidelines from LLVM forbid the use of the <iostream> library,
//     so the header of the equivalent LLVM functionality must be included.

class Parser {
    Lexer &lex_;     // used to retrieve the next token from the input
    Token tok_;      // stores the next token (the look-ahead)
    bool has_error_; // indicates if an error was detected

    void error() {
        llvm::errs() << "Unexpected: " << tok_.getText() << "\n";
        has_error_ = true;
    }

    // advance() retrieves the next token from the lexer.expect()
    // tests whether the look-ahead is of the expected kind and emits an error message if not
    void advance() { lex_.next(tok_); }

    bool expect(Token::TokenKind kind) {
        if (tok_.getKind() != kind ) {
            // llvm::outs() << tok_.getKind() << " != " << kind << "\n";
            error();
            return true;
        }
        return false;
    }

    // consume() retrieves the next token if the look-ahead is of the expected kind. 
    // If an error message is emitted, the has_error_ flag is set to true.
    bool consume(Token::TokenKind kind) {
        if (expect(kind))
            return true;
        advance();
        return false;
    }

    AST  *parseCalc();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();
    // There are no methods for ident and number. 
    //     Those rules only return the token and are replaced by the corresponding token.

public:
    Parser(Lexer &lex): lex_(lex), has_error_(false) {
        // llvm::outs() << "Parser::Parser\n";
        advance();
    }

    bool hasError() { return has_error_; }

    // the parse() method is the main entry point into parsing
    AST *parse();
};