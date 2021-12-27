#include "parser.h"
#include "name.h"

#include <string>
namespace compiler {
    Parser::TokenType Parser::MovetoNextToken() noexcept(false)
    {
        using namespace std;
        const char*& bp{ cp_ };

        while (isspace(*bp))
            bp++;

        if (isdigit(*bp) || *bp == '.') {
            number_ = strtod(bp, (char**)&cp_);
            token_ = Parser::NUM;
        }
        // The judgment of the name should be after the judgment of the number, 
        // because the number is a subset of the name
        else if (isalnum(*bp) || *bp == '_') {
            int     len = strspn(bp, kAlphaNumber);
            string  name(bp, len);
            bp += len;

            token_ = Screen(name);
        }
        else if (*bp == '<') {
            if (*(bp + 1) != 0 && *(bp + 1) == '=') {
                token_ = Parser::LE;
                bp += 2;
            }
            else {
                token_ = Parser::LT;
                bp++;
            }
        }
        else if (*bp == '>') {
            if (*(bp + 1) != 0 && *(bp + 1) == '=') {
                token_ = Parser::GE;
                bp += 2; // >= be sure there is no space between > and =
            }
            else {
                token_ = Parser::GT;
                bp++;
            }
        }
        else if (*bp == '=') {
            if (*(bp + 1) != 0 && *(bp + 1) == '=') {
                token_ = Parser::EQ;
                bp += 2; // >= be sure there is no space between > and =
            }
            else {
                token_ = Parser::ASIGN;
                bp++;
            }
        }

        else if (*bp != 0) {
            token_ = static_cast<TokenType>(*cp_);
            bp++;
        }
        else {
            if (MovetoNextLineIfExist())
                token_ = MovetoNextToken();
            else
            {
                return Parser::END;
            }
        }
        return token();
    }

    bool Parser::MovetoNextLineIfExist(std::string line) noexcept(false) {
        if (++line_num <= source.size() - 1) {
            line_ = source.at(line_num);
            cp_ = line_.c_str();
            return true;
        }
        return false;
    }

    void Parser::InsertORAssign(compiler_name_ptr_t name) {
        symbol_table_.insert_or_assign(name->name, name);
    }

    Parser::compiler_name_ptr_t Parser::SearchName(std::string name) {
        if (auto  f = symbol_table_.find(name); f != symbol_table_.end() && f->second)
            return f->second;
        else
            return nullptr;
    }

    Parser::TokenType Parser::Screen(std::string name) noexcept(false) {
        name_ = name;
        if (auto search = symbol_table_.find(name); search != symbol_table_.end()) {
            auto [n, p] = *search;
            return static_cast<TokenType>(p->token);
        }
        // default behaviour, if we haven't meet the name, it should be a name
        else {
            return Parser::NAME;
        }
    }

    void Parser::initSymbolTable() {
        std::vector<compiler_name_ptr_t> names;
        names.push_back(std::make_shared<compiler::Name>(
            std::string("let"), Parser::LET));

        for (auto name : names) {
            InsertORAssign(name);
        }
    }
}