#include <iostream>
#include <fstream>
#include "lex.h"
#include "parse.h"
#include "ast.h"

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

    auto ast = parse->ParseMain();

    std::cout << ast->getBody().size() << std::endl;
    std::cout << ast->getBody()[0]->toString() << std::endl;
    std::cout << ((lang::FunctionCallAST*)ast->getBody()[1].get())->getArguments()[0]->toString() << std::endl;
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
    run_parse_test();

    return 0;
}