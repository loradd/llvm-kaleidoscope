#include "AST.h"
#include "../utils/Utils.h"
#include "../logger/Logger.h"
#include "llvm/IR/BasicBlock.h"

llvm::Value *ast::NumberLiteral::codegen() {
    return llvm::ConstantFP::get(utils::context, llvm::APFloat(value));
}

llvm::Value *ast::VariableReference::codegen() {
    llvm::Value *symbol = utils::symbols[variable];
    if (!symbol) {
        return logger::log_value_error("unknown variable name");
    }
    return symbol;
}

llvm::Value *ast::BinaryOperation::codegen() {
    llvm::Value *lhs_value = lhs -> codegen();
    llvm::Value *rhs_value = rhs -> codegen();
    if (!lhs_value || !rhs_value) {
        return nullptr;
    }
    switch (binary_operator) {
        // addition
        case '+' :
            return utils::builder.CreateFAdd(lhs_value, rhs_value, "addtmp");
        // subtraction
        case '-' :
            return utils::builder.CreateFSub(lhs_value, rhs_value, "subtmp");
        // multiplication
        case '*' :
            return utils::builder.CreateFMul(lhs_value, rhs_value, "multmp");
        // comparison
        case '<' :
            return utils::builder.CreateUIToFP(
                utils::builder.CreateFCmpULT(lhs_value, rhs_value, "cmptmp"),
                    llvm::Type::getDoubleTy(utils::context), "booltmp");
        // unknown
        default :
            return logger::log_value_error("invalid binary operator");
    }
}

llvm::Value *ast::FunctionCall::codegen() {
    // retrieve function from module
    llvm::Function *callee_function = utils::module -> getFunction(callee);
    if (!callee_function) {
        return logger::log_value_error("unknown referenced function");
    }
    // check arguments
    if (callee_function -> arg_size() != arguments.size()) {
        return logger::log_value_error("incorrect # of arguments");
    }
    // generate code for arguments
    std::vector<llvm::Value *> argument_values;
    for (int i = 0; i < arguments.size(); i++) {
        argument_values.push_back(arguments[i] -> codegen());
        if (!argument_values.back()) {
            return nullptr;
        }
    }
    // generate code for function call
    return utils::builder.CreateCall(callee_function, argument_values, "calltmp");
}

llvm::Function *ast::FunctionDeclaration::codegen() {
    // create vector of arguments.size double values
    std::vector<llvm::Type *> doubles(arguments.size(), llvm::Type::getDoubleTy(utils::context));
    // create function return type (always double in kaleidoscope)
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(utils::context), doubles, false);
    // create function
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, name, utils::module.get());
    // set names for all arguments
    unsigned index = 0;
    for (auto &argument : function -> args()) {
        argument.setName(arguments[index++]);
    }
    // return function declaration
    return function;
} 

llvm::Function *ast::FunctionDefinition::codegen() {
    // retrieve function declaration from previous externs
    llvm::Function *function_definition = utils::module -> getFunction(declaration -> get_name());
    // generate code for function declaration (if not done yet)
    if (!function_definition) {
        function_definition = declaration -> codegen();
    }
    // error if null
    if (!function_definition) {
        return nullptr;
    }
    // error function redefinition
    if (!function_definition -> empty()) {
        return (llvm::Function *) logger::log_value_error("Function cannot be redefined.");
    }
    // create new basic block for function body
    llvm::BasicBlock *function_body = llvm::BasicBlock::Create(utils::context, "entry", function_definition);
    // move builder to function body
    utils::builder.SetInsertPoint(function_body);
    // clear symbols
    utils::symbols.clear();
    // add arguments to symbols
    for (auto &function_argument : function_definition -> args()) {
        utils::symbols[function_argument.getName()] = &function_argument;
    }
    // generate code from body, set return value and check
    if (llvm::Value *function_return_value = body -> codegen()) {
        utils::builder.CreateRet(function_return_value);
        llvm::verifyFunction(*function_definition);
        return function_definition;
    }
    // error, remove function from module
    function_definition -> eraseFromParent();
    return nullptr;
}
