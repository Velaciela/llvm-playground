#include "code_gen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespase {

class ToIRVisitor : public ASTVisitor {

    Module *m_;
    IRBuilder<> builder_;
    Type *void_ty_;
    Type *int32_ty_;
    Type *int8_ptr_ty_;
    Type *int8_ptr_ptr_ty_;
    Constant *int32_zero_; 
    Value *v_;
    StringMap<Value *> name_map_;

public:
    ToIRVisitor(Module *m) : m_(m), Builder(m->getContext()) {
        void_ty_ = Type::getVoidTy(m->getContext());
        int32_ty_ = Type::getInt32Ty(m->getContext());
        int8_ptr_ty_ = Type::getInt8PtrTy(m->getContext());
        int8_ptr_ptr_ty_ = int8_ptr_ty_->getPointerTo());
        int32_zero_ = ConstantInt::get(int32_ty_, 0, true);
    }

    void run(AST *tree) {
        FunctionType *main_fty = FunctionType::get(int32_ty_, {int32_ty_, int8_ptr_ptr_ty_}, false);
        Function *main_fn = Function::Create(main_fty, GlobalValue::ExternalLinkage, "main", m_);
        
        BasicBlock *bb = BasicBlock::Create(m_->getContext(), "entry", main_fn);
        builder_.SetInsertPoint(bb);
        tree->accept(*this);

        FunctionType *calc_write_fn_ty = FunctionType::get(void_ty_, {int32_ty_}, false);
        Function *calc_write_fn = Function::Create(calc_write_fn_ty, GlobalValue::ExternalLinkage, "calc_write", m_);
        builder_.CreateCall(calc_write_fn_ty, calc_write_fn, {v_});

        builder_.CreateRet(int32_zero_);
    }

    virtual void visit(WithDecl &node) override {
        FunctionType *reaf_fty = FunctionType::get(int32_ty_, {int8_ptr_ty}, false);
        Function *read_fn = Function::Create(reaf_fty, GlobalValue::ExternalLinkage, "calc_read", m_);

        for (auto i = node.begin(), e = node.end(); i != e; ++i) {
            StringRef var = *i;
            Constant *str_text = ConstantDataArray::getString(m_->getContext(), var);
            GlobalVariable *str = new GlobalVariable(*m_, str_text->getType(),
                                                    /*isConstant*/true,
                                                    GlobalValue::PrivateLinkage,
                                                    str_text, Twine(var).concat(".str"));
            Value *ptr = Builder.CreateInBoundsGEP(
                str, {int32_zero_, int32_zero_}, "ptr");
            CallInst *call = Builder.CreateCall(read_fty, read_fn, {ptr});
            name_map_[var] = call;
        }

        node.getExpr()->accept(*this);
    }

    virtual void visit(Factor &node) override {
        if (node.getKind() == Factor::Ident) {
            v = name_map_[node.getValue()];
        }
        else {
            int int_val;
            node.getVal().getAsInterger(10, int_val);
            v = ConstantInt::get(int32_ty_, int_val, true);
        }
    }

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


void CodeGen::compile(AST *tree) {
    LLVMContext ctx;
    Module *m = new Module("calc.expr", ctx);
    ToIRVisitor to_ir(m);
    to_ir.run(tree);
    m->print(outs(), nullptr);
}