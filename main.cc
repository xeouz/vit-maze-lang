#include <iostream>
#include <fstream>

#define IMPL_LANG_DEFS
#define IMPL_LANG_SYSLIB
#include "lex.h"
#include "parse.h"
#include "ast.h"
#include "interpret.h"
#include "langlib.h"

class MathLib: xeouz::FCIFunctionLibraryBase
{
    LIBRARY_BEGIN(MathLib)
    ADD_FUNCTION(add, ARG("a", NUMBER), ARG("b", NUMBER)) RETURNS(NUMBER)
    LIBRARY_END()

    FUNCTION add(ARGUMENTS args)
    {
        double a, b;
        a = args["a"]->getAsNumber()->getValue();
        b = args["b"]->getAsNumber()->getValue();

        double ret = a + b;
        return CREATE_NUMBER(ret);
    }
};

using namespace xeouz;
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
    std::ifstream file("../in/test.xeouz");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    auto lex = xeouz::Lexer::create(text);
    auto parse = xeouz::Parser::create(std::move(lex));
    auto it = xeouz::Interpreter::create(std::move(parse));
    xeouz::lib::registerLibraries(it);

    it->interpretMain();
}

void run_parse_test()
{
    std::string text;
    std::ifstream file("../in/test.xeouz");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    auto lex = std::make_unique<xeouz::Lexer>(text);
    auto parse = std::make_unique<xeouz::Parser>(std::move(lex));

    auto a = std::make_unique<xeouz::VariableNumberData>(1002.3);
    std::cout << a->getValue() << std::endl;
}

void run_lex_test()
{
    std::string text;
    std::ifstream file("../in/test.xeouz");

    std::string line;
    while(getline(file, line))
    {
        text += line + "\n";
    }

    xeouz::Lexer lex(text);

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