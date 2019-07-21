#include "Logger.h"

std::unique_ptr<ast::Expression> logger::log_expression_error(const char *error) {
    fprintf(stderr, "LogError: %s\n", error);
    return nullptr;
}

std::unique_ptr<ast::FunctionDeclaration> logger::log_function_declaration_error(const char *error) {
    log_expression_error(error);
    return nullptr;
}

llvm::Value *logger::log_value_error(const char *error) {
    log_expression_error(error);
    return nullptr;
}