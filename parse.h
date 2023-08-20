#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "lex.h"
#include "ast.h"

namespace lang
{

static std::unordered_map<std::string, int> ParserPrecedenceMap = {
    {"<", 10},
    {">", 10},
    {"<=", 10},
    {">=", 10},
    {"==", 10},
    {"!=", 10},
    {"+", 20},
    {"-", 20},
    {"*", 40},
    {"/", 40},
    {"%", 40},
    {"**", 60}
};

class Parser
{
    std::unique_ptr<Lexer> lexer;
    bool parse_success;

    void getNextTokenUnchecked(bool first_token = false);
public:
    std::unique_ptr<Token> current_token;

    Parser(std::unique_ptr<Lexer> lexer);

    std::unique_ptr<ASTBase> LogError(std::string const& error, bool should_set_success=true);
    bool const getParseSuccess() const;

    void getNextToken(int token_type);
    std::unique_ptr<Token> copyCurrentToken();

    std::unique_ptr<ASTBase> ParsePrimary();
    std::unique_ptr<ASTBase> ParseExpression();
    std::unique_ptr<FunctionCallAST> ParseFunctionCall(std::unique_ptr<Token> name);

    std::unique_ptr<ASTBase> ParseIdentifier(bool atsign = false, bool ignore_assignment = false);
    std::unique_ptr<NumberAST> ParseNumber();
    std::unique_ptr<StringAST> ParseString();

    std::unique_ptr<ASTBase> ParseParenthesis();
    std::unique_ptr<ASTBase> ParseBinaryOperation(int precedence, std::unique_ptr<ASTBase> lhs);

    std::unique_ptr<VariableDefinitionAST> ParseVariableDefinition();
    std::unique_ptr<VariableAssignmentAST> ParseVariableAssignment(std::unique_ptr<ASTBase> expression);
    std::unique_ptr<VariableAssignmentAST> ParseShorthandVariableAssignment(std::unique_ptr<ASTBase> expression);

    std::unique_ptr<SequenceAST> ParseSequence();
    std::unique_ptr<ASTBase> ParseDo();

    std::unique_ptr<ExternAST> ParseExtern();

    std::unique_ptr<NumberAST> ParseTrueFalse();
    std::unique_ptr<IfAST> ParseIf();
    std::unique_ptr<ASTBase> ParseIfElse();

    std::unique_ptr<MainAST> ParseMain(std::string const& program_name = "main");

    static std::unique_ptr<Parser> create(std::unique_ptr<Lexer> lexer);
};

}