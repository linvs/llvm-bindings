#include <llvm/Support/FileSystem.h>
#include "IR/index.h"
#include "Util/index.h"

void Module::Init(Napi::Env env, Napi::Object &exports) {
    Napi::HandleScope scope(env);

    Napi::Object flagBehaviorNS = Napi::Object::New(env);
    flagBehaviorNS.Set("Error", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Error));
    flagBehaviorNS.Set("Warning", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Warning));
    flagBehaviorNS.Set("Require", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Require));
    flagBehaviorNS.Set("Override", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Override));
    flagBehaviorNS.Set("Append", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Append));
    flagBehaviorNS.Set("AppendUnique", Napi::Number::New(env, llvm::Module::ModFlagBehavior::AppendUnique));
    flagBehaviorNS.Set("Max", Napi::Number::New(env, llvm::Module::ModFlagBehavior::Max));
    flagBehaviorNS.Set("ModFlagBehaviorFirstVal", Napi::Number::New(env, llvm::Module::ModFlagBehavior::ModFlagBehaviorFirstVal));
    flagBehaviorNS.Set("ModFlagBehaviorLastVal", Napi::Number::New(env, llvm::Module::ModFlagBehavior::ModFlagBehaviorLastVal));

    Napi::Function func = DefineClass(env, "Module", {
            InstanceMethod("empty", &Module::empty),
            InstanceMethod("getFunction", &Module::getFunction),
            InstanceMethod("getOrInsertFunction", &Module::getOrInsertFunction),
            InstanceMethod("getGlobalVariable", &Module::getGlobalVariable),
            InstanceMethod("getTypeByName", &Module::getTypeByName),
            InstanceMethod("getName", &Module::getName),
            InstanceMethod("getDataLayout", &Module::getDataLayout),
            InstanceMethod("setDataLayout", &Module::setDataLayout),
            InstanceMethod("getModuleIdentifier", &Module::getModuleIdentifier),
            InstanceMethod("setModuleIdentifier", &Module::setModuleIdentifier),
            InstanceMethod("getSourceFileName", &Module::getSourceFileName),
            InstanceMethod("setSourceFileName", &Module::setSourceFileName),
            InstanceMethod("getTargetTriple", &Module::getTargetTriple),
            InstanceMethod("setTargetTriple", &Module::setTargetTriple),
            InstanceMethod("print", &Module::print),
            InstanceMethod("addModuleFlag", &Module::addModuleFlag),
            StaticValue("ModFlagBehavior", flagBehaviorNS)
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Module", func);
}

bool Module::IsClassOf(const Napi::Value &value) {
    return value.As<Napi::Object>().InstanceOf(constructor.Value());
}

Napi::Object Module::New(Napi::Env env, llvm::Module *module) {
    return constructor.New({Napi::External<llvm::Module>::New(env, module)});
}

llvm::Module *Module::Extract(const Napi::Value &value) {
    if (value.IsNull()) {
        return nullptr;
    }
    return Unwrap(value.As<Napi::Object>())->getLLVMPrimitive();
}

Module::Module(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    Napi::Env env = info.Env();
    if (!info.IsConstructCall()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::constructor);
    }
    unsigned argsLen = info.Length();
    if (argsLen >= 1 && info[0].IsExternal()) {
        auto external = info[0].As<Napi::External<llvm::Module>>();
        module = external.Data();
        return;
    } else if (argsLen >= 2 && info[0].IsString() && LLVMContext::IsClassOf(info[1])) {
        const std::string &moduleID = info[0].As<Napi::String>();
        llvm::LLVMContext &context = LLVMContext::Extract(info[1]);
        module = new llvm::Module(moduleID, context);
        return;
    }
    throw Napi::TypeError::New(env, ErrMsg::Class::Module::constructor);
}

llvm::Module *Module::getLLVMPrimitive() {
    return module;
}

Napi::Value Module::empty(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, module->empty());
}

Napi::Value Module::getFunction(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::getFunction);
    }
    std::string functionName = info[0].As<Napi::String>();
    llvm::Function *function = module->getFunction(functionName);
    if (function) {
        return Function::New(env, function);
    }
    return env.Null();
}

Napi::Value Module::getOrInsertFunction(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsString() || !FunctionType::IsClassOf(info[1])) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::getOrInsertFunction);
    }
    std::string functionName = info[0].As<Napi::String>();
    llvm::FunctionType *funcType = FunctionType::Extract(info[1]);
    llvm::FunctionCallee callee = module->getOrInsertFunction(functionName, funcType);
    return FunctionCallee::New(env, callee);
}

Napi::Value Module::getGlobalVariable(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString() || info.Length() >= 2 && !info[1].IsBoolean()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::getGlobalVariable);
    }
    const std::string &name = info[0].As<Napi::String>();
    bool allowInternal = false;
    if (info.Length() >= 2) {
        allowInternal = info[1].As<Napi::Boolean>();
    }
    llvm::GlobalVariable *global = module->getGlobalVariable(name, allowInternal);
    if (global) {
        return GlobalVariable::New(env, global);
    }
    return env.Null();
}

Napi::Value Module::getTypeByName(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::getTypeByName);
    }
    const std::string name = info[0].As<Napi::String>();
    llvm::StructType *type = llvm::StructType::getTypeByName(module->getContext(), name);
    if (type) {
        return StructType::New(env, type);
    }
    return env.Null();
}

Napi::Value Module::getName(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, module->getName().str());
}

Napi::Value Module::getDataLayout(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    const llvm::DataLayout &dataLayout = module->getDataLayout();
    return DataLayout::New(env, const_cast<llvm::DataLayout *>(&dataLayout));
}

void Module::setDataLayout(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString() && DataLayout::IsClassOf(info[0])) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::setDataLayout);
    }
    if (info[0].IsString()) {
        const std::string &desc = info[0].As<Napi::String>();
        module->setDataLayout(desc);
    } else if (DataLayout::IsClassOf(info[0])) {
        llvm::DataLayout &dl = DataLayout::Extract(info[0]);
        module->setDataLayout(dl);
    }
}

Napi::Value Module::getModuleIdentifier(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, module->getModuleIdentifier());
}

void Module::setModuleIdentifier(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::setModuleIdentifier);
    }
    const std::string &moduleID = info[0].As<Napi::String>();
    module->setModuleIdentifier(moduleID);
}

Napi::Value Module::getSourceFileName(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, module->getSourceFileName());
}

void Module::setSourceFileName(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::setSourceFileName);
    }
    const std::string &sourceFileName = info[0].As<Napi::String>();
    module->setSourceFileName(sourceFileName);
}

Napi::Value Module::getTargetTriple(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, module->getTargetTriple());
}

void Module::setTargetTriple(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::setTargetTriple);
    }
    const std::string &triple = info[0].As<Napi::String>();
    module->setTargetTriple(triple);
}

void Module::print(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    unsigned argsLen = info.Length();
    if (argsLen >= 1 && !info[0].IsString()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::Module::print);
    }
    if (argsLen == 0) {
        module->print(llvm::outs(), nullptr);
    } else {
        std::string filename = info[0].As<Napi::String>();
        std::error_code errorCode;
        llvm::raw_fd_ostream outfile(filename, errorCode, llvm::sys::fs::OF_Text);
        if (errorCode) {
            throw Napi::TypeError::New(env, errorCode.message() + ": " + filename);
        }
        module->print(outfile, nullptr);
    }
}

void Module::addModuleFlag(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 3 && info[0].IsNumber() && info[1].IsString() && info[2].IsNumber()) {
        unsigned behaviorNum = info[0].As<Napi::Number>();
        if (behaviorNum < llvm::Module::ModFlagBehaviorFirstVal || behaviorNum > llvm::Module::ModFlagBehaviorLastVal) {
            throw Napi::TypeError::New(env, ErrMsg::Class::Module::addModuleFlag);
        }
        auto behavior = llvm::Module::ModFlagBehavior(behaviorNum);
        const std::string &key = info[1].As<Napi::String>();
        unsigned value = info[2].As<Napi::Number>();
        module->addModuleFlag(behavior, key, value);
        return;
    }
    throw Napi::TypeError::New(env, ErrMsg::Class::Module::addModuleFlag);
}
