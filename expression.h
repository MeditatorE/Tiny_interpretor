#pragma once


#ifndef OOC_ARITHMETIC_EXPRESSION_H_
#define OOC_ARITHMETIC_EXPRESSION_H_

#include "parser.h"

#include <memory>
#include <string>


namespace compiler {

class Expression;

typedef  std::shared_ptr<Expression> expression_ptr_t;

class Expression{

public:

    const expression_ptr_t left_;
    const expression_ptr_t right_;

    virtual double exec() const { return 0; }
    virtual ~Expression() {}
    Expression() {}
    Expression(const expression_ptr_t l, const expression_ptr_t r) : left_(l), right_(r) {}
    virtual expression_ptr_t left()  { return left_; }
    virtual expression_ptr_t right() { return right_; }
    virtual std::string op() { return "null"; }
};

class AddExpression : public Expression {
public:
    AddExpression(const expression_ptr_t l, const expression_ptr_t r)
      :Expression(l, r)  {}
    double exec() const override { return left_->exec() + right_->exec(); }
    virtual std::string op() { return "+"; }
private:
};

class SubExpression : public Expression {
public:
    SubExpression(const expression_ptr_t l, const expression_ptr_t r) : Expression(l, r) {}
    double exec()const override { return left_->exec() - right_->exec(); }
    virtual std::string op() { return "-"; }
};

class MulExpression : public Expression {
public:
    MulExpression(const expression_ptr_t &l, const expression_ptr_t &r) :Expression(l, r) {}
    double exec() const  override { return left_->exec() * right_->exec(); }
    virtual std::string op() { return "*"; }
};

class DivExpression : public Expression {
public:
    DivExpression(const expression_ptr_t & l, const expression_ptr_t & r) :Expression(l, r) {}
    double exec() const  override { return left_->exec() / right_->exec(); }
    virtual std::string op() { return "/"; }
};

class ValueExpression : public Expression {
public:
    ValueExpression(double value) : value_(value){}
    double exec() const  override { return value_; }
    virtual std::string op() { return std::to_string(value_); }
private:
    double value_;
};

class MinusExpression : public Expression {
public:
    MinusExpression(expression_ptr_t value) : value_(value) {}
    double exec() const  override { return -1 * value_->exec(); }
    virtual std::string op() { return std::to_string(-1*value_->exec()); }
private:
    const expression_ptr_t value_;
};


class CompareExpression : public Expression {
    using operator_specifier_t = std::string;

public:
    CompareExpression(const expression_ptr_t& l, const expression_ptr_t& r, 
        const operator_specifier_t operator_str)
        :Expression(l, r) {  
        this->operator_str = operator_str;
    }
    double exec() const  override { 
        if (operator_str == ">") 
            return left_->exec() > right_->exec();
        else if (operator_str == "<")
            return left_->exec() < right_->exec();
        else if (operator_str == "<=")
            return left_->exec() <= right_->exec();
        else if (operator_str == ">=")
            return left_->exec() >= right_->exec();
        else
            return 0;
    }

    virtual std::string op() { return operator_str; }
private:
    operator_specifier_t operator_str;
};


class TernaryExpression : public Expression {

public:
    TernaryExpression(const expression_ptr_t judge, const expression_ptr_t l,
        const expression_ptr_t r)
        :Expression(l, r), judge_(judge) {
    }
    double exec() const  override {
        if (judge_->exec() > 0) {
            return left_->exec();
        }
        else {
            return right_->exec();
        }
    }
    virtual std::string op() { return "?:"; }
private:
    const expression_ptr_t judge_;
};

class Parser;

expression_ptr_t  factor(Parser &p);
expression_ptr_t  sum(Parser &p);
expression_ptr_t  product(Parser &p);
expression_ptr_t  compare(Parser& p);
expression_ptr_t  ternary(Parser& p);

expression_ptr_t  assign(Parser& p);
expression_ptr_t  statement(Parser& p);

void printExpression(expression_ptr_t treenode, int depth = 0);

} // namespace compiler

#endif // !OOC_ARITHMETIC_EXPRESSION_H_