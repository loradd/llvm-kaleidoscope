#ifndef __PARSER_H__
#define __PARSER_H__

#include <map>
#include "../ast/AST.h"

namespace parser {

    extern std::map<char, int> binary_operator_precedences;

    std::unique_ptr<ast::Expression> parse_expression();
    std::unique_ptr<ast::Expression> parse_binary_operation_rhs(int previous_token_precedence, std::unique_ptr<ast::Expression> lhs);
    std::unique_ptr<ast::Expression> parse_primary_expression();
    std::unique_ptr<ast::Expression> parse_number_expression();
    std::unique_ptr<ast::Expression> parse_parenthesized_expression();
    std::unique_ptr<ast::Expression> parse_identifier_expression();
    std::unique_ptr<ast::FunctionDeclaration> parse_function_declaration();
    std::unique_ptr<ast::FunctionDefinition> parse_function_definition();
    std::unique_ptr<ast::FunctionDeclaration> parse_extern_function();
    std::unique_ptr<ast::FunctionDefinition> parse_top_level_expression();

}

#endif