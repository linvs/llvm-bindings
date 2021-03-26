#include "IR/ConstantInt.h"
#include "IR/Constant.h"
#include "IR/LLVMContext.h"
#include "IR/Type.h"
#include "IR/IntegerType.h"
#include "ADT/APInt.h"
#include "Util/Inherit.h"

void ConstantInt::Init(Napi::Env env, Napi::Object &exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "ConstantInt", {
            StaticMethod("get", &ConstantInt::get),
            StaticMethod("getTrue", &ConstantInt::getTrue),
            StaticMethod("getFalse", &ConstantInt::getFalse)
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    Inherit(env, constructor.Value(), Constant::constructor.Value());
    exports.Set("ConstantInt", func);
}

Napi::Object ConstantInt::New(Napi::Env env, llvm::ConstantInt *constantInt) {
    return constructor.New({Napi::External<llvm::ConstantInt>::New(env, constantInt)});
}

bool ConstantInt::IsClassOf(const Napi::Value &value) {
    return value.As<Napi::Object>().InstanceOf(constructor.Value());
}

llvm::ConstantInt *ConstantInt::Extract(const Napi::Value &value) {
    return Unwrap(value.As<Napi::Object>())->getLLVMPrimitive();
}

ConstantInt::ConstantInt(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    Napi::Env env = info.Env();
    if (!info.IsConstructCall()) {
        throw Napi::TypeError::New(env, "Class Constructor ConstantInt cannot be invoked without new");
    }
    if (info.Length() != 1 || !info[0].IsExternal()) {
        throw Napi::TypeError::New(env, "ConstantInt constructor requires: constant: External");
    }
    auto external = info[0].As<Napi::External<llvm::ConstantInt>>();
    constantInt = external.Data();
}

llvm::ConstantInt *ConstantInt::getLLVMPrimitive() {
    return constantInt;
}

Napi::Value ConstantInt::get(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    int argsLen = info.Length();
    if (argsLen == 2 && LLVMContext::IsClassOf(info[0]) && APInt::IsClassOf(info[1])) {
        llvm::LLVMContext &context = LLVMContext::Extract(info[0]);
        llvm::APInt &value = APInt::Extract(info[1]);
        return ConstantInt::New(env, llvm::ConstantInt::get(context, value));
    } else if (argsLen >= 2 && IntegerType::IsClassOf(info[0]) && info[1].IsNumber()) {
        llvm::IntegerType *intType = IntegerType::Extract(info[0]);
        uint64_t value = info[1].As<Napi::Number>().Int64Value();
        bool isSigned = argsLen == 3 && info[2].As<Napi::Boolean>();
        return ConstantInt::New(env, llvm::ConstantInt::get(intType, value, isSigned));
    } else if (argsLen >= 2 && Type::IsClassOf(info[0])) {
        llvm::Type *type = Type::Extract(info[0]);
        if (APInt::IsClassOf(info[1])) {
            llvm::APInt &value = APInt::Extract(info[1]);
            return Constant::New(env, llvm::ConstantInt::get(type, value));
        } else if (info[1].IsNumber()) {
            uint64_t value = info[1].As<Napi::Number>().Int64Value();
            bool isSigned = argsLen == 3 && info[2].As<Napi::Boolean>();
            return Constant::New(env, llvm::ConstantInt::get(type, value, isSigned));
        }
    }
    throw Napi::TypeError::New(env, "ConstantInt::get needs to be called with: (context: LLVMContext)");
}

Napi::Value ConstantInt::getTrue(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1 || !LLVMContext::IsClassOf(info[0])) {
        throw Napi::TypeError::New(env, "ConstantInt::getTrue needs to be called with: (context: LLVMContext)");
    }
    llvm::LLVMContext &context = LLVMContext::Extract(info[0]);
    llvm::ConstantInt *constantInt = llvm::ConstantInt::getTrue(context);
    return ConstantInt::New(env, constantInt);
}

Napi::Value ConstantInt::getFalse(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1 || !LLVMContext::IsClassOf(info[0])) {
        throw Napi::TypeError::New(env, "ConstantInt::getFalse needs to be called with: (context: LLVMContext)");
    }
    llvm::LLVMContext &context = LLVMContext::Extract(info[0]);
    llvm::ConstantInt *constantInt = llvm::ConstantInt::getFalse(context);
    return ConstantInt::New(env, constantInt);
}