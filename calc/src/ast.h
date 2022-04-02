#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;

class ASTVisitor {
// The visitor pattern needs to know each class it must visit. 
// Because each class also refers to the visitor, 
//     we declare all the classes at the top of the file. 
// Note that the visit() methods for AST and Expr have a default implementation, which does nothing.
public:
    virtual void visit(AST &) {};
    virtual void visit(Expr &) {};
    virtual void visit(Factor &) = 0;
    virtual void visit(BinaryOp &) = 0;
    virtual void visit(WithDecl &) = 0;
};

// The AST class is the root of the hierarchy
class AST {
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor &v) = 0;
};

// Expr is the root for the AST classes related to expressions
class Expr : public AST {
public:
    Expr() {}
};

// The Factor class stores a number or the name of a variable
class Factor : public Expr {
public:
    enum ValueKind { Ident, Number };

private:
    ValueKind kind_;
    llvm::StringRef val_;

public:
    Factor(ValueKind kind, llvm::StringRef val) : kind_(kind), val_(val) {}
    ValueKind getKind() { return kind_; }
    llvm::StringRef getVal() { return val_; }
    virtual void accept(ASTVisitor &v) override {
        v.visit(*this);
    }
};

// In this example, numbers and variables are treated almost identically, 
// so we create only one AST node class to represent them. 
// The 'kind_' member tells us which of both cases the instances represent. 
// In more complex languages, you usually want to have different AST classes, 
//     such as a NumberLiteral class for numbers 
//     and a VariableAccess class for a reference to a variable.


// The BinaryOp class holds the data that's needed to evaluate an expression
// The BinaryOp class makes no distinction between multiplicative and additive operators. 
// The precedence of the operators is implicitly available in the tree structure.
class BinaryOp : public Expr {

public:
    enum Operator { Plus, Minus, Mul, Div };

private:
    Expr *left_;
    Expr *right_;
    Operator op_;

public:
    BinaryOp(Operator op, Expr *l, Expr *r)
        : op_(op), left_(l), right_(r) {}
    Expr *getLeft() { return left_; }
    Expr *getRight() { return right_; }
    Operator getOperator() { return op_; }
    virtual void accept(ASTVisitor &v) override {
        v.visit(*this);
    }
};


// WithDecl stores the declared variables and the expression
class WithDecl : public AST {
    using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
    VarVector vars_;
    Expr *e_;

public:
    WithDecl(llvm::SmallVector<llvm::StringRef,8> vars, Expr *e)
        : vars_(vars), e_(e) 
    {
        // llvm::outs() << "WithDecl: " << "\n";
        for (auto itr = vars.begin(); itr < vars.end(); itr++) {
            // llvm::outs() << "    " << *itr << "\n";
        }
    }
    VarVector::const_iterator begin() {return vars_.begin(); }
    VarVector::const_iterator end() { return vars_.end(); }
    Expr *getExpr() { return e_; }
    virtual void accept(ASTVisitor &v) override {
        v.visit(*this);
    }
};


// The AST is constructed during parsing.

// The semantic analysis checks that the tree adheres to the meaning of the language 
//     (for example, that used variables are declared) 
//     and possibly augments the tree. 

// After that, the tree is used for code generation.

