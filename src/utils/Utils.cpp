#include "Utils.h"

// This is an object that owns LLVM core data structures
llvm::LLVMContext utils::context;

// This is an helper object that makes easy to generate LLVM instructions
llvm::IRBuilder<> utils::builder(context);

// This is an LLVM construct that contains functions and global variables
std::unique_ptr<llvm::Module> utils::module;

// This map keeps tracks of which values are defined in the current scope
std::map<std::string, llvm::Value *> utils::symbols;