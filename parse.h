#include <memory>

#include "lex.h"
#include "ast.h"

namespace lang
{

class Parser
{
    std::unique_ptr<Lexer> lexer;
public:
    std::unique_ptr<Token> current_token;

    Parser(std::unique_ptr<Lexer> lexer);

    std::unique_ptr<ASTBase> LogError(std::string const& error);
};

}