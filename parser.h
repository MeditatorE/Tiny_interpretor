#pragma once

#ifndef OOC_ARITHMETIC_PARSER_H_
#define OOC_ARITHMETIC_PARSER_H_

#include "expression.h"
#include <cstring>

#include <fstream>
#include <string>
#include <string_view>
#include <map>
#include <vector>

namespace compiler {

class Name;

class Parser
{
    using token_number_value_t = double;
    using compiler_name_ptr_t = std::shared_ptr<compiler::Name>;
public:
    const char* kAlphaNumber = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz" 
        "_" "0123456789";

    enum TokenType {
        MUL = '*', ADD = '+', SUB = '-', Div = '/', STATEMENT_END =';',
        END = 0 , LParent = '(', RParent = ')',
        GT = '>', LT = '<', EQ = 'e', 
        QUESTION = '?', COLON=':',
        VAR = 'v', GE = 'g', LE = 'c', NUM = 'n', ASIGN = 'a',
        LET = 'l', NAME = 'b'
    };

    //Parser(const std::ifstream&  _file);
    Parser(std::vector<std::string> s)
        :token_(TokenType::END), number_(0), cp_(0), line_(""), source(s) {

        if (source.size() > 0) {
            line_ = source.at(line_num);
            this->cp_ = this->line_.c_str();
        }
        initSymbolTable();
    }

    // when never your call this function, you would get a new token
    TokenType    token() const { return token_; }

    TokenType    PeekToken(int num = 1) { 

        auto copy{ *this };
        return copy.MovetoNextToken();

    }
    TokenType&   token() { return token_; }
    std::string  line() { return line_; }
    int          Index() { return cp_-line_.c_str(); }
    std::string  name() const { return name_; }
    // return a number if the token type is a number type
    token_number_value_t  number() const { return number_; }
    //expression_ptr_t  symbol();

    

    void InsertORAssign(compiler_name_ptr_t name);
    compiler_name_ptr_t SearchName(std::string name);
    TokenType Screen(std::string name) noexcept(false);
    TokenType MovetoNextToken() noexcept(false);
    bool MovetoNextLineIfExist(std::string line="") noexcept(false) ;

private:
    
    void initSymbolTable();
    
    std::vector<std::string> source;
    std::map<std::string, compiler_name_ptr_t> symbol_table_;
    std::string             name_;

    const char*             cp_; // index of next char 
    int                     line_num; // index of current line
    std::string             line_; // current line
    TokenType               token_; 
    token_number_value_t    number_;// Valid when token_ is num
};



#define ErrorMsg(p, fmt) {                                          \
    const int    buf_size{ 1024 };                                  \
    char         buffer[buf_size] = { 0 };                          \
    std::string  invalid_loc(p.Index(), ' '); invalid_loc += '^';     \
    std::snprintf(buffer, buf_size, fmt                             \
        R"(    "%s" )" "\n"                                         \
        R"(     %s)" "\n", p.line().c_str(), invalid_loc.c_str());  \
    throw std::runtime_error(buffer); }                             \

//void  Parser::MovetoNextLine() {
//
//}


#endif // OOC_ARITHMETIC_PARSER_H_

}