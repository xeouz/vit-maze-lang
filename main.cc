#include <iostream>
#include <fstream>
#include "lex.h"

void run_test_file()
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
    run_test_file();

    return 0;
}