// include utils
#include "utils/Utils.h"

// include lexer
#include "lexer/Lexer.h"

// include ast
// #include "ast/BinaryExpression.h"
// #include "ast/Expression.h"
// #include "ast/FunctionDefinition.h"
// #include "ast/FunctionCall.h"
// #include "ast/FunctionPrototype.h"
// #include "ast/NumberLiteral.h"
// #include "ast/VariableReference.h"

// include logger
#include "logger/Logger.h"

// include parser
#include "parser/Parser.h"

using namespace llvm;

static void handle_function_definition() {
  if (auto ast = parser::parse_function_definition()) {
    if (auto *ir = ast -> codegen()) {
      fprintf(stderr, "Parsed a function definition:");
      ir -> print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    // skip token for error recovery
    lexer::get_next_token();
  }
}

static void handle_extern_function() {
  if (auto ast = parser::parse_extern_function()) {
    if (auto *ir = ast -> codegen()) {
      fprintf(stderr, "Parsed an extern function:");
      ir -> print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    // skip token for error recovery
    lexer::get_next_token();
  }
}

static void handle_top_level_expression() {
  if (auto ast = parser::parse_top_level_expression()) {
    if (auto *ir = ast -> codegen()) {
      fprintf(stderr, "Read top level expression:");
      ir -> print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    // skip token for error recovery
    lexer::get_next_token();
  }
}

static void main_loop() {
  while (1) {
    fprintf(stderr, "> " );
    switch (lexer::current_token) {
      case lexer::Token::END_OF_FILE:
        return;
      case ';':
        lexer::get_next_token();
        break;
      case lexer::Token::DEFINITION:
        handle_function_definition();
        break;
      case lexer::Token::EXTERN:
        handle_extern_function();
        break;
      default:
        handle_top_level_expression();
        break;
    }
  }
}

static void init_binary_operator_precedences () {
  // install standard binary operators
  // higher value <=> higher precedence
  parser::binary_operator_precedences['<'] = 10;
  parser::binary_operator_precedences['+'] = 20;
  parser::binary_operator_precedences['-'] = 20;
  parser::binary_operator_precedences['*'] = 30; // highest precedence
}

int main() {
  
  init_binary_operator_precedences();
  
  fprintf(stderr, "> ");
  lexer::get_next_token();

  utils::module = llvm::make_unique<llvm::Module>("Kaleidoscope JIT", utils::context);

  main_loop();

  utils::module->print(errs(), nullptr);

  return 0;
}
