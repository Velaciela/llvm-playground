#pragma once

#include "llvm/Support/MemoryBuffer.h" 
    // provides read-only access to a block of memory
    //    On request, a trailing zero character ('\x00') is added to the end of the buffer. 
    //    We use this feature to read through the buffer without checking the length of the buffer at each access

#include "llvm/ADT/StringRef.h"
    // encapsulates a pointer to a C string and its length. 
    //    Because the length is stored, the string doesn't need to be terminated with a zero character ('\x00') like normal C strings. 
    //    This allows an instance of StringRef to point to the memory managed by MemoryBuffer. 



class Lexer;

class Token {
    friend class Lexer;

public:
    enum TokenKind : unsigned short {
        eoi, // end of input 
        unknown, // error
        ident,
        number,
        comma,
        colon,
        plus,
        minus,
        star,
        slash,
        l_paren,
        r_paren,
        KW_with
    };

private:
    TokenKind kind_; // a unique number to each token to make handling them easier
    llvm::StringRef text_; // points to the start of the text of the token

public:
    TokenKind getKind() const { return kind_; }
    llvm::StringRef getText() const { return text_;}

    bool is(TokenKind k) const { return kind_ == k; }
    bool isOneOf(TokenKind k1, TokenKind k2) const { return is(k1) || is(k2); }

    // variadic template, which allows a variable number of arguments
    template <typename ... Ts>
    bool isOneOf(TokenKind k1, TokenKind k2, Ts ... Ks) const {
         return is(k1) || isOneOf(k2, Ks ...); 
    }
};

class Lexer{
    const char *buffer_start_;
    const char *buffer_ptr_;

public:
    Lexer(const llvm::StringRef &buffer) {
        buffer_start_ = buffer.begin();
        buffer_ptr_ = buffer_start_;
    }

    void next(Token &token);

private:
    void formToken(Token &result, const char *tok_end, Token::TokenKind kind);

};