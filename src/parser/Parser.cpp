#include "Parser.h"

#include "../logger/Logger.h"
#include "../lexer/Lexer.h"

std::map<char, int> parser::binary_operator_precedences;

// retrieve operator precedence
static int get_current_token_precedence() {
    // -1 if it is not an ascii character
    if (!isascii(lexer::current_token)) {
        return -1;
    }
    // retrieve token precedence 
    int token_precedence = parser::binary_operator_precedences[lexer::current_token];
    if (token_precedence <= 0) {
        return -1;
    }
    // return token precedence
    return token_precedence;
}

// parse expression
std::unique_ptr<ast::Expression> parser::parse_expression() {
    // parse primary expression
    auto primary_expression = parse_primary_expression();
    if (!primary_expression) {
        return nullptr;
    }
    // parse possible right hand side if binary operation
    return parse_binary_operation_rhs(0, std::move(primary_expression));
}

// parse binary expression right hand side
std::unique_ptr<ast::Expression> parser::parse_binary_operation_rhs(
    int previous_token_precedence, std::unique_ptr<ast::Expression> lhs
) {
    while (1) {
        // retrieve current token precedence
        int current_token_precedence = get_current_token_precedence();
        // lewer precedence - return left hand side
        if (current_token_precedence < previous_token_precedence) {
            return lhs;
        }
        // retrieve operator
        int binary_operator = lexer::current_token;
        // consume operator
        lexer::get_next_token();
        // parse right hand side
        auto rhs = parse_primary_expression();
        if (!rhs) {
            return nullptr;
        }
        // retrieve next token precedence
        int next_token_precedence = get_current_token_precedence();
        // parse higer precedence right hand side expression
        if (current_token_precedence < next_token_precedence) {
            // parse right hand side with higher precedence
            rhs = parse_binary_operation_rhs(
                current_token_precedence + 1, std::move(rhs)
            );
            if (!rhs) {
                return nullptr;
            }
        }
        // update left hand side expression
        // lhs = llvm::make_unique<ast::BinaryOperation>(binary_operator, std::move(lhs), std::move(rhs));
        lhs = llvm::make_unique<ast::BinaryOperation>(binary_operator, std::move(lhs), std::move(rhs));
    }
}

// parse primary expression
std::unique_ptr<ast::Expression> parser::parse_primary_expression() {
    switch (lexer::current_token) {
        case lexer::Token::IDENTIFIER:
            return parse_identifier_expression();
        case lexer::Token::NUMBER:
            return parse_number_expression();
        case '(':
            return parse_parenthesized_expression();
        default: 
            return logger::log_expression_error("unknown token when expecting an expression");
    }
}

// Parse number literal
std::unique_ptr<ast::Expression> parser::parse_number_expression() {
    // create ast node
    auto number_literal = llvm::make_unique<ast::NumberLiteral>(lexer::number);
    // consume number
    lexer::get_next_token();
    // return node
    return std::move(number_literal);
}

// Parse parenthesized expression
std::unique_ptr<ast::Expression> parser::parse_parenthesized_expression() {
    // consume '('
    lexer::get_next_token();
    // parse inner expression
    auto expression = parse_expression();
    if (!expression) {
        return nullptr;
    }
    // consume ')', throw error if absent
    if (lexer::current_token != ')') {
        return logger::log_expression_error("expected ')'");
    }
    lexer::get_next_token();
    // return inner node
    return expression;
}

std::unique_ptr<ast::Expression> parser::parse_identifier_expression() {
    // retrieve name
    std::string identifier = lexer::identifier;
    // consume variable name
    lexer::get_next_token();
    // variable reference
    if (lexer::current_token != '(') {
        return llvm::make_unique<ast::VariableReference>(identifier);
    }
    // function call, consume '('
    lexer::get_next_token();
    // parse function call arguments
    std::vector<std::unique_ptr<ast::Expression>> arguments;
    if (lexer::current_token != ')') {
        while (1) {
            if (auto argument = parse_expression()) {
                arguments.push_back(std::move(argument));
            } else {
                return nullptr;
            }
            // break if no more parameters
            if (lexer::current_token == ')') {
                break;
            }
            // error if no other argument
            if (lexer::current_token != ',') {
                return logger::log_expression_error("expected ')' or ',' in argument list");
            }
            // consume ','
            lexer::get_next_token();
        } 
    }
    // consume ')'
    lexer::get_next_token();
    // return function call node
    return llvm::make_unique<ast::FunctionCall>(identifier, std::move(arguments));
}

std::unique_ptr<ast::FunctionDeclaration> parser::parse_function_declaration() {
    // error if no identifier
    if (lexer::current_token != lexer::Token::IDENTIFIER) {
        return logger::log_function_declaration_error("expected name in function prototype");
    }
    // retrieve name
    std::string identifier = lexer::identifier;
    // consume function name
    lexer::get_next_token();
    // error if no left parenthesis
    if (lexer::current_token != '(') {
        return logger::log_function_declaration_error("expected '(' in function prototype");
    }
    // parse arguments
    std::vector<std::string> arguments;
    while (lexer::get_next_token() == lexer::Token::IDENTIFIER) {
        arguments.push_back(lexer::identifier);
    }   
    // error if no closing parenthesis
    if (lexer::current_token != ')') {
        return logger::log_function_declaration_error("expected ')' in function prototype");
    }
    // consume ')'
    lexer::get_next_token();
    // return function prototype node
    return llvm::make_unique<ast::FunctionDeclaration>(identifier, arguments); 
}

// parse function definition
std::unique_ptr<ast::FunctionDefinition> parser::parse_function_definition() {
    // consume 'def'
    lexer::get_next_token();
    // parse function prototype
    auto declaration = parse_function_declaration();
    if (!declaration) {
        return nullptr;
    }
    // parse function body
    if (auto body = parse_expression()) {
        return llvm::make_unique<ast::FunctionDefinition>(std::move(declaration), std::move(body));
    }
    return nullptr;
}

// parse extern function
std::unique_ptr<ast::FunctionDeclaration> parser::parse_extern_function() {
    // consume 'extern'
    lexer::get_next_token();
    // parse function prototype
    return parse_function_declaration();
}

// top level expressions - zero-argument anonymous functions
std::unique_ptr<ast::FunctionDefinition> parser::parse_top_level_expression() {
    if (auto expression = parse_expression()) {
        // wrap expression into nullary anonymous function
        auto declaration = llvm::make_unique<ast::FunctionDeclaration>("", std::vector<std::string>());
        return llvm::make_unique<ast::FunctionDefinition>(std::move(declaration), std::move(expression));
    }
    return nullptr;
}