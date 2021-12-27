#pragma once

#ifndef NAME_H_
#define NAME_H_

#include "parser.h"
#include "expression.h"

#include <string>
#include <stdexcept>
#include <exception>

namespace compiler {

    class Name : public Expression {

        typedef  std::string        name_str_t;
        typedef Parser::TokenType   token_t;

    public:
        name_str_t name;
        token_t token;
        Name(name_str_t namestr, token_t tk, const expression_ptr_t r = nullptr) 
            : Expression(nullptr, r), name(namestr), token(tk) {

        }
        virtual double exec() const  override {
            throw std::runtime_error("this function should not be called."); 
        }
    };

    class Assign : public Name {

        typedef  std::string        name_str_t;
        typedef Parser::TokenType   token_t;
    public:
        Assign(name_str_t namestr, token_t tk, const expression_ptr_t r)
            :Name(namestr, tk, r) {
        }
        virtual double exec() const  override { return this->right_->exec(); }
        virtual std::string op() { return "="; }
    };

}




#endif // !NAME_H_
