#include <iostream>
#include <fstream>
#include "lex.h"

void run_parse_test()
{

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