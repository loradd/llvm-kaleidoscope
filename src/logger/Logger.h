#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "../ast/AST.h"
#include "llvm/IR/Value.h"

namespace logger {

    std::unique_ptr<ast::Expression> log_expression_error(const char *error);
    std::unique_ptr<ast::FunctionDeclaration> log_function_declaration_error(const char *error);
    llvm::Value *log_value_error(const char *error);

}

#endif