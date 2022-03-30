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
// a simple approach called panic mode must be used.
//     In panic mode, tokens are deleted from the token stream 
//         until one is found that the parser can use to continue its work. 
//     Most programming languages have symbols that denote an end; 
//         for example, in C++, we can use ; (end of a statement) or } (end of a block). 
//         Such tokens are good candidates to look for.
_error:
    while (!tok_is(Token::eoi))
        advance();
    return nullptr;
}


// The other parsing methods are constructed in a similar fashion. 
// parseExpr() is the translation of the rule for expr
//     expr : term (( "+" | "-" ) term)* ;
Expr *Parser::parseExpr() {
    Expr *left = parseTerm();
    while (tok_.isOneOf(Token::plus, Token::minus)) {
        // repeated group inside the rule is translated into a while loop.
        // note how the use of the isOneOf() method simplifies the check for several tokens.
        BinaryOp::operator op = tok_.is(Token::plus) ?
                                BinaryOp::Plus : BinaryOp::Minus;
        advance();
        Expr *right = parseTerm();
        left = new BinaryOp(op, left, right);
    }
    return left;
}


// term : factor (( "*" | "/") factor)* ;
Expr *Parser::parseTerm() {
    Expr *left = parseFactor();
    while (tok_.isOneOf(Token::star, Token::slash)) {
        BinaryOp::Operator op = tok_.is(Token::star) ?
                                BinaryOp::Mul : BinaryOp::Div;
        advance();
        Expr *right = parseFactor();
        left = new BinaryOp(op, left, right);
    }
    return left;


// factor : ident | number | "(" expr ")" ;
Expr *Parser::parseFactor() {
    Expr *res = nullptr;
    switch (tok_.getKind()) {
        case Token::number:
            res = new Factor(Factor::Number, Tok.getText());
            advance(); break;
        case Token::ident:
            res = new Factor(Factor::Ident, Tok.getText());
            advance(); break;
        case Token::l_paren:
            advance(); 
            res = parseExpr();
            if (!consume(Token::r_paren)) break;
        default:
            if (!res) error();
            while (!tok_.isOneOf(Token::r_paren, 
                                 Token::star,
                                 Token::plus,
                                 Token::minus,
                                 Token::slash,
                                 Token::eoi))
                advance();
    }
    return res;
}


// As soon as you have memorized the used patterns, 
// it is almost tedious to code the parser based on the grammar rules. 
// This type of parser is called a recursive descent parser.