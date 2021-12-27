#include "expression.h"
#include "name.h"

#include <iostream>
#include <exception>
#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>


namespace compiler {

    expression_ptr_t factor(Parser& p)
    {
        auto pe = std::make_shared<Expression>();

        switch (p.token())
        {
        case Parser::ADD:
        {
            p.MovetoNextToken();
            return factor(p);
        }
        case Parser::SUB:
        {
            p.MovetoNextToken();
            return std::make_shared<MinusExpression>((factor(p)));
        }
        case Parser::VAR:
        {
            pe = p.SearchName(p.name());
            if (!pe) {
                ErrorMsg(p, "undefined symbol: ")
            }
            break;
        }
        case Parser::NUM:
        {
            double d{ static_cast<double>(p.number()) };
            pe = std::make_shared<ValueExpression>(d);
            break;
        }
        case Parser::LParent:
        {
            p.MovetoNextToken();
            pe = assign(p);
            if (Parser::RParent != p.token()) {
                const int    buf_size{ 1024 };
                char         buffer[buf_size] = { 0 };
                std::string  invalid_loc(p.Index(), ' '); invalid_loc += '^';

                std::snprintf(buffer, buf_size, R"(Expecting ')' character here:)" "\n"
                    R"(    "%s" )" "\n"
                    R"(     %s)" "\n", p.line().c_str(), invalid_loc.c_str());
                throw std::runtime_error(buffer);
            }
            break;
        }
        default:
        {
            /* const int    buf_size{ 1024 };
             char         buffer[buf_size] = { 0 };
             std::string  invalid_loc(p.idx(), ' '); invalid_loc += '^';

             if (p.token() != Parser::END) {

                 std::snprintf(buffer, buf_size, "Oops, unexpected character here:\n"
                     R"(    "%s" )" "\n"
                     R"(     %s)" "\n", p.line().c_str(), invalid_loc.c_str());
             }
             else {
                 std::snprintf(buffer, buf_size, "Oops, unexpected endding here:\n"
                     R"(    "%s" )" "\n"
                     R"(     %s)" "\n", p.line().c_str(), invalid_loc.c_str());
             }*/
            if (p.token() != Parser::END) {
                ErrorMsg(p, "Oops, unexpected character here:\n");
            }
            else
            {
                ErrorMsg(p, "Oops, unexpected endding here:\n");
            }
            break;
        }
        }
        p.MovetoNextToken();
        return pe;
    }

    expression_ptr_t product(Parser& p) {

        auto result = factor(p);
        char token = '0';

        for (;;) {
            switch (p.token())
            {
            case Parser::MUL:
                token = '*';
                break;
            case Parser::Div:
                token = '/';
                break;
            default:
                return result;
            }
            p.MovetoNextToken();
            if (token == '*')
                result = std::make_unique<MulExpression>(result, factor(p));
            else
                result = std::make_unique<DivExpression>(result, factor(p));
        }
    }
    expression_ptr_t  sum(Parser& p) {

        auto result = product(p);
        char token = '0';

        for (;;) {
            switch (p.token())
            {
            case Parser::ADD:
                token = '+';
                break;
            case Parser::SUB:
                token = '-';
                break;
            default:
                return result;
            }
            p.MovetoNextToken();

            if (token == '+')
                result = std::make_unique<AddExpression>(result, product(p));
            else
                result = std::make_unique<SubExpression>(result, product(p));
        }
    }

    expression_ptr_t  compare(Parser& p) {

        auto result = sum(p);
        std::string token = ">";

        for (;;) {
            switch (p.token())
            {
            case Parser::GT:
                token = ">";
                break;
            case Parser::LT:
                token = "<";
                break;
            case Parser::GE:
                token = ">=";
                break;
            case Parser::LE:
                token = "<=";
                break;
            case Parser::EQ:
                token = "==";
                break;
            default:
                return result;
            }
            p.MovetoNextToken();
            result = std::make_unique<CompareExpression>(result, sum(p), token);
        }
    }

    expression_ptr_t  ternary(Parser& p) {
        auto result = compare(p);

        switch (p.token())
        {
        case Parser::QUESTION:
        {
            p.MovetoNextToken();
            auto left = compare(p);
            if (p.token() == Parser::COLON)
            {
                p.MovetoNextToken();
                auto right = compare(p);
                return std::make_unique<TernaryExpression>(result, left, right);
            }
            else
            {
                ErrorMsg(p, "another expression expected");
            }
        }
        default:
            return result;
        }
        /*p.MovetoNextToken();
        result = std::make_unique<CompareExpression>(result, sum(p), token);*/
    }


    expression_ptr_t  assign(Parser& p) {

        switch (p.token())
        {
        case Parser::VAR:
        {
            auto varname{ p.name() };
            if (p.PeekToken() != Parser::ASIGN) {
                // if the variable is a factor
                return ternary(p);
            }
            else {
                p.MovetoNextToken(); // 此时指向等号
                p.MovetoNextToken(); // 此时为下一次调用assign做好准备
                auto result = std::make_shared<compiler::Assign>(varname, Parser::VAR, assign(p));
                p.InsertORAssign(result);
                return result;
            }
        }
        default:
            return ternary(p);
        }
    }

    expression_ptr_t  statement(Parser& p) {

        switch (p.token())
        {
        case Parser::LET:
        {
            if (p.MovetoNextToken() != Parser::NAME) {
                ErrorMsg(p, "expect new variable name \n");
            }
            auto key{ p.name() };

            if (p.MovetoNextToken() != Parser::ASIGN) {
                ErrorMsg(p, "expect assign operator \n");
            }

            p.MovetoNextToken();
            auto result = std::make_shared<compiler::Assign>(key, Parser::VAR, assign(p));
            //!!!! auto result = std::make_shared<compiler::Assign>(p.name(), Parser::VAR, assign(p));
            p.InsertORAssign(result);

            if (p.token() != Parser::STATEMENT_END) {
                ErrorMsg(p, "semicolon ; ecpected\n");
            }
            // 如果是语句结束的符号，那么就让解析向前，相当于这个分号已经被吃掉了
            // 其他的地方也是这样
            return result;
        }
        default:
            auto result = assign(p);

            if (p.token() != Parser::STATEMENT_END) {
                ErrorMsg(p, "semicolon ; ecpected\n");
            }
            return result;
        }

    }

    void printExpression(expression_ptr_t treenode, int depth) {
        if (treenode->left()) {
            printExpression(treenode->left(), depth + 1);
        }
        std::string operator_str(depth * 4, ' ');
        operator_str.append(treenode->op());
        std::cout << operator_str << std::endl;
        if (treenode->right()) {
            printExpression(treenode->right(), depth + 1);
        }
    }

}