#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

namespace ast {

    class Expression {
        public:
            virtual ~Expression() {}
            virtual llvm::Value *codegen() = 0;
    };

    class NumberLiteral : public Expression {

        double value;

        public:
            NumberLiteral(double value) 
                : value(value) {}
            virtual llvm::Value *codegen();
    };

    class VariableReference : public Expression {

        std::string variable;

        public:
            VariableReference(std::string variable) 
                : variable(variable) {}
            virtual llvm::Value *codegen();

    };

    class FunctionCall : public Expression {

        std::string callee;
        std::vector<std::unique_ptr<Expression>> arguments;

        public:
            FunctionCall(std::string callee, std::vector<std::unique_ptr<Expression>> arguments) 
                : callee(callee), arguments(std::move(arguments)) {}
            virtual llvm::Value *codegen();  

    };

    class BinaryOperation : public Expression {
        
        char binary_operator;
        std::unique_ptr<Expression> lhs, rhs;

        public:
            BinaryOperation(char binary_operator, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
                : binary_operator(binary_operator), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
            virtual llvm::Value *codegen();

    };

    // function declaration
    class FunctionDeclaration {

        std::string name;
        std::vector<std::string> arguments;

        public:
            FunctionDeclaration(const std::string name, std::vector<std::string> arguments)
                : name(name), arguments(std::move(arguments)) {}
            virtual llvm::Function *codegen();
            const std::string &get_name() const { return name; }
    };

    // function definition
    class FunctionDefinition {
        
        std::unique_ptr<FunctionDeclaration> declaration;
        std::unique_ptr<Expression> body;

        public:
            FunctionDefinition(std::unique_ptr<FunctionDeclaration> declaration, std::unique_ptr<Expression> body)
                : declaration(std::move(declaration)), body(std::move(body)) {}
            virtual llvm::Function *codegen();

    };

}

#endif