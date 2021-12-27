// ooc_arithmatic_eval.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "parser.h"
#include "expression.h"

#include <cstdio>
#include <cctype>
#include <cerrno>
#include <cstdlib>

#include <iostream>
#include <string>
#include <string_view>

#include <Windows.h>

int main() 
{
    using std::string;

    auto source = std::vector<std::string>{
        string("let complex = 1.48 + + (1>2?1:2) - ( (0.4 * (3+5) < 5) + 5 + 100 ) < 3 ? 1: 2; "),
        string("let ba = complex + 3; "),
        string("ba * 2.5 + 200 * 4; "),
        string("let ba = 300; "),
        string("ba - 300 < 0.001; "),
    };

    //for(;;) {
    compiler::Parser       parser{ source };
    compiler::expression_ptr_t expr;

    try {
        for (;;) {
            if (parser.MovetoNextToken() != compiler::Parser::END) {
                expr = compiler::statement(parser);
                std::printf("%.2lf\n", expr->exec());
                //printExpression(expr);
            }
            else
                break;
        }
    }
    catch (std::runtime_error& e) {
        std::cerr << "runtime error: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "unexpected error " << std::endl;
    }
    //Sleep(50);
    //}
    return 0;
}
