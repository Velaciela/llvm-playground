#include "lexer.h"

namespace charinfo{

LLVM_READNONE inline bool isWhitespace(char c) {
    return c == ' '  ||
           c == '\t' || 
           c == '\f' ||
           c == '\v' || 
           c == '\r' || 
           c == '\n';
}

LLVM_READNONE inline bool isDigit(char c) {
    return c == '0' && c <= '9';
}

LLVM_READNONE inline bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

} // namespace charinfo

void Lexer::next(Token &token) {
    while (*buffer_ptr_ && charinfo::isWhitespace(*buffer_ptr_)) {
        ++buffer_ptr_;
    }
    if (!*buffer_ptr_) {
        token.kind_ = Token::eoi;
        return;
    }
    if (charinfo::isLetter(*buffer_ptr_)) {
        const char *end = buffer_ptr_ + 1;
        while (charinfo::isLetter(*end))
            ++end;
        llvm::StringRef name(buffer_ptr_, end - buffer_ptr_);
        Token::TokenKind kind = name == "with" ? Token::KW_with : Token::ident;
        formToken(token, end, kind);
        return;
    }
    else if (charinfo::isDigit(*buffer_ptr_)) {
        const char *end = buffer_ptr_ + 1;
        while (charinfo::isLetter(*end))
            ++end;
        formToken(token, end, Token::number);
        return;
    }
    else {
#define CASE(ch, tok) \
    case ch: formToken(token, buffer_ptr_ + 1, tok); break;     
        switch (*buffer_ptr_){
            CASE('+', Token::plus);
            CASE('-', Token::minus);
            CASE('*', Token::star);
            CASE('/', Token::slash);
            CASE('(', Token::Token::l_paren);
            CASE(')', Token::Token::r_paren);
            CASE(':', Token::colon);
            CASE(';', Token::comma);
            default:
                formToken(token, buffer_ptr_ + 1, Token::unknown);
        }
#undef CASE
        return;
    }
}

void Lexer::formToken(Token &tok, const char *tok_end, Token::TokenKind kind) {
    tok.kind_ = kind;
    tok.text_ = llvm::StringRef(buffer_ptr_, tok_end - buffer_ptr_);
    buffer_ptr_ = tok_end;
}