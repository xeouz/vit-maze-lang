#include <iostream>
#include <fstream>

#define IMPL_LANG_DEFS
#include "lex.h"
#include "parse.h"
#include "ast.h"
#include "interpret.h"
#include "langlib.h"

class MathLib: public lang::FCIFunctionLibraryBase
{
    LIBRARY_BEGIN(MathLib)
    ADD_FUNCTION(max, ARG("a", NUMBER), ARG("b", NUMBER)) RETURNS(NUMBER)
    LIBRARY_END()
    
    FUNCTION max(ARGUMENTS args)
    {
        double a = args["a"]->getAsNumber()->getValue();
        double b = args["b"]->getAsNumber()->getValue();

        if(a>b)
            return CREATE_NUMBER(a);
        else
            return CREATE_NUMBER(b);
    }
};

void run_interpret_test()
{
    std::string text;
    std::ifstream file("../in/test.lang");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    auto lex = lang::Lexer::create(text);
    auto parse = lang::Parser::create(std::move(lex));
    auto it = lang::Interpreter::create(std::move(parse));
    lang::lib::registerLibraries(it);
    it->registerFunctionLibrary<MathLib>();

    it->interpretMain();
}

void run_parse_test()
{
    std::string text;
    std::ifstream file("../in/test.lang");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    auto lex = std::make_unique<lang::Lexer>(text);
    auto parse = std::make_unique<lang::Parser>(std::move(lex));

    auto a = std::make_unique<lang::VariableNumberData>(1002.3);
    std::cout << a->getValue() << std::endl;
}

void run_lex_test()
{
    std::string text;
    std::ifstream file("../in/test.lang");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    lang::Lexer lex(text);

    while(lex.getIndex() < text.length()-1)
    {
        auto tok = lex.getToken();
        std::cout << *tok << std::endl;
    }
}

int main()
{
    run_interpret_test();

    return 0;
}