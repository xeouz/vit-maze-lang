#include <iostream>
#include <fstream>

#define IMPL_LANG_DEFS
#define IMPL_LANG_SYSLIB
#include "lex.h"
#include "parse.h"
#include "ast.h"
#include "interpret.h"
#include "langlib.h"

class MathLib: lang::FCIFunctionLibraryBase
{
    LIBRARY_BEGIN(MathLib)
    ADD_FUNCTION(add, ARG("a", ANY), ARG("b", ANY)) RETURNS(NUMBER)
    LIBRARY_END()

    FUNCTION add(ARGUMENTS args)
    {
       lang::VariableDataBase* a, * b;
        a = args["a"];
        b = args["b"];

        if(a->getType() == lang::VT_NUMBER)
        {
            std::cout << "ADDING NUMBERS" << std::endl;
        }
        else if(a->getType() == lang::VT_STRING)
        {
            std::cout << "ADDING STRINGS" << std::endl;
        }

        return CREATE_NUMBER(10);
    }
};

using namespace lang;
void run_test()
{
    // Get file text
    std::string text;
    std::ifstream file("../in/test.lang");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    // Setup Aphel
    auto interpreter = Interpreter::create(text);

    // Add Libraries
    interpreter->registerFunctionLibrary<MathLib>();
    lib::registerLibraries(interpreter);
    
    // Run Aphel
    interpreter->interpretMain();
}

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
    run_test();

    return 0;
}