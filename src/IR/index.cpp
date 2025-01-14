#include "IR/index.h"

void InitIR(Napi::Env env, Napi::Object &exports) {
    LLVMContext::Init(env, exports);
    Module::Init(env, exports);
    Type::Init(env, exports);
    IntegerType::Init(env, exports);
    FunctionType::Init(env, exports);
    FunctionCallee::Init(env, exports);
    StructType::Init(env, exports);
    ArrayType::Init(env, exports);
    PointerType::Init(env, exports);
    Value::Init(env, exports);
    Argument::Init(env, exports);
    BasicBlock::Init(env, exports);
    User::Init(env, exports);
    Constant::Init(env, exports);
    ConstantInt::Init(env, exports);
    ConstantFP::Init(env, exports);
    ConstantStruct::Init(env, exports);
    ConstantPointerNull::Init(env, exports);
    ConstantExpr::Init(env, exports);
    UndefValue::Init(env, exports);
    GlobalValue::Init(env, exports);
    GlobalObject::Init(env, exports);
    GlobalVariable::Init(env, exports);
    Function::Init(env, exports);
    Instruction::Init(env, exports);
    AllocaInst::Init(env, exports);
    StoreInst::Init(env, exports);
    LoadInst::Init(env, exports);
    CallInst::Init(env, exports);
    PHINode::Init(env, exports);
    LandingPadInst::Init(env, exports);
    ReturnInst::Init(env, exports);
    BranchInst::Init(env, exports);
    SwitchInst::Init(env, exports);
    InvokeInst::Init(env, exports);
    ResumeInst::Init(env, exports);
    UnreachableInst::Init(env, exports);
    IRBuilder::Init(env, exports);
    Metadata::Init(env, exports);
    MDNode::Init(env, exports);
    LLVMConstants::Init(env, exports);
    DITypeRefArray::Init(env, exports);
    DebugLoc::Init(env, exports);
    DINode::Init(env, exports);
    DIScope::Init(env, exports);
    DIFile::Init(env, exports);
    DIType::Init(env, exports);
    DIBasicType::Init(env, exports);
    DIDerivedType::Init(env, exports);
    DICompositeType::Init(env, exports);
    DISubroutineType::Init(env, exports);
    DICompileUnit::Init(env, exports);
    DILocalScope::Init(env, exports);
    DILocation::Init(env, exports);
    DISubprogram::Init(env, exports);
    DILexicalBlock::Init(env, exports);
    DINamespace::Init(env, exports);
    DIVariable::Init(env, exports);
    DIExpression::Init(env, exports);
    DIGlobalVariable::Init(env, exports);
    DILocalVariable::Init(env, exports);
    DIGlobalVariableExpression::Init(env, exports);
    DIBuilder::Init(env, exports);
    DataLayout::Init(env, exports);
    Verifier::Init(env, exports);
    Intrinsic::Init(env, exports);
}
