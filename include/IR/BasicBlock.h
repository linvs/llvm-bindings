#pragma once

#include <napi.h>
#include <llvm/IR/BasicBlock.h>

class BasicBlock : public Napi::ObjectWrap<BasicBlock> {
public:
    static inline Napi::FunctionReference constructor; // NOLINT

    static void Init(Napi::Env env, Napi::Object &exports);

    static Napi::Object New(Napi::Env env, llvm::BasicBlock *basicBlock);

    static bool IsClassOf(const Napi::Value &value);

    static llvm::BasicBlock *Extract(const Napi::Value &value);

    explicit BasicBlock(const Napi::CallbackInfo &info);

    llvm::BasicBlock *getLLVMPrimitive();

private:
    llvm::BasicBlock *basicBlock = nullptr;

    static Napi::Value create(const Napi::CallbackInfo &info);
};