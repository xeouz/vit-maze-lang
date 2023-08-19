#include <iostream>
#include <fstream>
#include "lex.h"
#include "parse.h"
#include "ast.h"
#include "interpret.h"
#include "langlib.h"

void run_interpret_test()
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
    auto it = std::make_unique<lang::Interpreter>(std::move(parse));
    lang::lib::registerLibraries(it.get());

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