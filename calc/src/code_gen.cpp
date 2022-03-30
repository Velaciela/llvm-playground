#include "code_gen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

// the AST contains the information from semantic analysis phase, 
//     the basic idea is to use a visitor to walk the AST

using namespace llvm;

namespase {

class ToIRVisitor : public ASTVisitor {

    // Each compilation unit is represented in LLVM by the Module class 
    //     and the visitor has a pointer to the module call, m_
    Module *m_;

    // For easy IR generation, the Builder (of the IRBuilder<> type) is used. 
    IRBuilder<> builder_;

    // LLVM has a class hierarchy to represent types in IR (these basic types are used often)
    // To avoid repeated lookup, we cache the needed type instance, which can be either:
    Type *void_ty_;
    Type *int32_ty_;
    Type *int8_ptr_ty_;
    Type *int8_ptr_ptr_ty_;
    Constant *int32_zero_; 
    
    // current calculated value, which is updated through tree traversal
    Value *v_; 
    
    // maps a variable name to the value that's returned by the calc_read() function
    StringMap<Value *> name_map_; 

public:

    ToIRVisitor(Module *m) : m_(m), Builder(m->getContext()) {
        // initializes all the members
        void_ty_ = Type::getVoidTy(m->getContext());
        int32_ty_ = Type::getInt32Ty(m->getContext());
        int8_ptr_ty_ = Type::getInt8PtrTy(m->getContext());
        int8_ptr_ptr_ty_ = int8_ptr_ty_->getPointerTo());
        int32_zero_ = ConstantInt::get(int32_ty_, 0, true);
    }


    void run(AST *tree) {
        // For each function, a FunctionType instance must be created. 
        // In C++ terminology, this is a function prototype. 
        // A function itself is defined with a Function instance. 
        // First, the run() method defines the main() function in LLVM IR:
        FunctionType *main_fty = FunctionType::get(int32_ty_, {int32_ty_, int8_ptr_ptr_ty_}, false);
        Function *main_fn = Function::Create(main_fty, GlobalValue::ExternalLinkage, "main", m_);
        
        // Then create the BB basic block with the entry label
        BasicBlock *bb = BasicBlock::Create(m_->getContext(), "entry", main_fn);
        // and attach it to the IR builder
        builder_.SetInsertPoint(bb);
        // With this preparation done, tree traversal can begin:
        tree->accept(*this); 

        // After tree traversal, the computed value is printed via a call to the calc_write() function. 
        // Again, a function prototype (an instance of FunctionType) must be created.
        FunctionType *calc_write_fn_ty = FunctionType::get(void_ty_, {int32_ty_}, false);
        Function *calc_write_fn = Function::Create(calc_write_fn_ty, GlobalValue::ExternalLinkage, "calc_write", m_);
        // The only parameter is the current value, v_:
        builder_.CreateCall(calc_write_fn_ty, calc_write_fn, {v_});

        // The generation finishes by returning a 0 from the main() function
        builder_.CreateRet(int32_zero_);
    }

    // A WithDecl node holds the names of the declared variables.
    virtual void visit(WithDecl &node) override {
        // First, we must create a function prototype for the calc_read() function:
        FunctionType *reaf_fty = FunctionType::get(int32_ty_, {int8_ptr_ty}, false);
        Function *read_fn = Function::Create(reaf_fty, GlobalValue::ExternalLinkage, "calc_read", m_);

        // The method loops through the variable names:
        for (auto i = node.begin(), e = node.end(); i != e; ++i) {
            // For each variable, a string with a variable name is created:
            StringRef var = *i;
            Constant *str_text = ConstantDataArray::getString(m_->getContext(), var);
            GlobalVariable *str = new GlobalVariable(*m_, str_text->getType(),
                                                    /*isConstant*/true,
                                                    GlobalValue::PrivateLinkage,
                                                    str_text, Twine(var).concat(".str"));
            
            // Then, the IR code to call the calc_read() function is created. 
            // The string that we created in the previous step is passed as a parameter:
            Value *ptr = Builder.CreateInBoundsGEP(
                str, {int32_zero_, int32_zero_}, "ptr");
            CallInst *call = Builder.CreateCall(read_fty, read_fn, {ptr});
            
            //The returned value is stored in the mapNames map for later use:
            name_map_[var] = call;
        }

        // Tree traversal continues with the expression:
        node.getExpr()->accept(*this);
    }

    // A Factor node is either a variable name or a number
    virtual void visit(Factor &node) override {
        if (node.getKind() == Factor::Ident) {
            // For a variable name, the value is looked up in the mapNames map. 
            v = name_map_[node.getValue()];
        }
        else {
            // For a number, the value is converted into an integer and turned into a constant value:
            int int_val;
            node.getVal().getAsInterger(10, int_val);
            v = ConstantInt::get(int32_ty_, int_val, true);
        }
    }

    // for a BinaryOp node, the right calculation operation must be used
    virtual void visit(BinaryOp &node) override {
        node.getLeft()->accept(*this);
        Value *left = v_;
        node.getRight()->accept(*this);
        Value *right = v;
        switch (node.getOperator()) {
            case BinaryOp::Plus:
                v_ = Builder.CreateNSWAdd(left, right); break;
            case BinaryOp::Minus:
                v_ = Builder.CreateNSWSub(left, right); break;
            case BinaryOp::Mul:
                v_ = Builder.CreateNSWMul(left, right); break;
            case BinaryOp::Div:
                v_ = Builder.CreateSDiv(left, right); break;
        }
    }
};

} // namespace


// The compile() method creates the global context and the module, 
// runs the tree traversal, and dumps the generated IR to the console:
void CodeGen::compile(AST *tree) {
    LLVMContext ctx;
    Module *m = new Module("calc.expr", ctx);
    ToIRVisitor to_ir(m);
    to_ir.run(tree);
    m->print(outs(), nullptr);
}