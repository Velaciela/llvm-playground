#include "sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

// The basic idea is that the name of each declared variable is stored in a set. 
// While the set it being created, we can check that each name is unique 
// and then check that the name is in the set later:

namespace {

class DeclCheck : public ASTVisitor {
    llvm::StringSet<> scope_; // A set-like wrapper for the StringMap.
    bool has_error_;

    enum ErrorType { Twice, Not };

    void error(ErrorType et, llvm::StringRef v) {
        llvm::errs() << "Variable '" << v << "' "
                     << (et == Twice ? "already" : "not")
                     << " declared\n";
        has_error_ = true;
    }

public:
    DeclCheck() : has_error_(false) {}
    bool hasError() { return has_error_; }
    
    // In a Factor node that holds a variable name, we check that the variable name is in the set
    virtual void visit(Factor &node) override {
        // llvm::outs() << "DeclCheck::Factor\n";
        if (node.getKind() == Factor::Ident) {
            // llvm::outs() << " value = " << node.getVal() << "\n";
            if (scope_.find(node.getVal()) == scope_.end())
                error(Not, node.getVal());
        }
    }

    // For a BinaryOp node, we only need to check that both sides exist and have been visited
    virtual void visit(BinaryOp &node) override {
        // llvm::outs() << "DeclCheck::BinaryOp\n";
        if (node.getLeft())
            node.getLeft()->accept(*this);
        else
            has_error_ = true;
        
        if (node.getRight())
            node.getRight()->accept(*this);
        else
            has_error_ = true;
    }

    // In a WithDecl node, the set is populated and the walk over the expression is started
    virtual void visit(WithDecl &node) override {
        // llvm::outs() << "DeclCheck::WithDecl\n";
        for (auto i = node.begin(), e = node.end(); i != e; ++i) {
            // llvm::outs() << "    " << *i << "\n";
            if (scope_.insert(*i).second == false)
                error(Twice, *i);
            /// insert - Insert the specified key/value pair into the map.  
            /// If the key already exists in the map, return false and ignore the request, 
            /// otherwise insert it and return true.
        }
        if (node.getExpr())
            node.getExpr()->accept(*this);
        else
            has_error_ = true;
    }

}; // class DeclCheck

} //namespace


// The semantic() method only starts the tree walk and returns an error flag
bool Sema::semantic(AST *tree) {
    // llvm::outs() << "Sema::semantic\n";
    if (!tree)
        return false;

    DeclCheck check;
    tree->accept(check);
    return check.hasError();
}
