#include "parse.h"
#include <iostream>

namespace lang
{

Parser::Parser(std::unique_ptr<Lexer> _lexer): lexer(std::move(_lexer))
{
    
}

std::unique_ptr<ASTBase> Parser::LogError(std::string const& error, bool should_set_success)
{
    if(should_set_success)
        parse_success = false;

    std::cout << error;
    return nullptr;
}
bool const Parser::getParseSuccess() const
{
    return parse_success;
}

void Parser::getNextTokenUnchecked(bool first_token)
{
    if(current_token && current_token->getTokenType()==T_EOF && !first_token)
    {
        LogError("PARSER: getNextTokenUnchecked(): End of file\n");
        return;
    }

    current_token.reset();
    current_token = lexer->getToken();

    if(!current_token)
    {
        LogError("PARSER: getNextTokenUnchecked(): Recieved Null Token\n");
        getNextTokenUnchecked();
    }
}
void Parser::getNextToken(int token_type)
{
    bool passed = true;
    if(current_token->getTokenType() != token_type)
        passed = false;
    if(current_token->getTokenType() == T_EOF && token_type == T_NEXTLINE)
        passed = true;

    if(!passed)
    {
        std::string error = "PARSER: getNextToken(): ";
        error += "Current Token type ";
        error += current_token->toString();
        error += " does not match expected type ";
        error += Token::GetStringFromType(token_type);
        error += "\n";
        LogError(error);
        parse_success = false;
    }

    getNextTokenUnchecked();
}

std::unique_ptr<Token> Parser::copyCurrentToken()
{
    return std::make_unique<Token>(current_token->getTokenType(), current_token->getValue(), current_token->line, current_token->charpos);
}

std::unique_ptr<ASTBase> Parser::ParsePrimary()
{
    switch(current_token->getTokenType())
    {
        case T_ATSIGN: return ParseIdentifier(true);
        case T_IDENTIFIER: return ParseIdentifier();

        case T_NUMBER: return ParseNumber();
        case T_STRING: return ParseString();

        case T_LPAREN: return ParseParenthesis();

        case T_LET: return ParseVariableDefinition();

        case T_LARROW: return ParseSequence();
        case T_DO: return ParseDo();

        case T_TRUE: return ParseTrueFalse();
        case T_FALSE: return ParseTrueFalse();
        case T_IF: return ParseIfElse();

        default: {
            parse_success = false;
            
            std::string error = "PARSER: ParsePrimary(): Unable to parse unexpected token ";
            error += current_token->toString();
            error += "\n";

            getNextTokenUnchecked();
            return LogError(error);
        }
    }
}
std::unique_ptr<ASTBase> Parser::ParseExpression()
{
    auto lhs = ParsePrimary();
    if(!lhs)
    {
        return LogError("PARSER: ParseExpression(): LHS was null\n");
    }

    return ParseBinaryOperation(0, std::move(lhs));
}
std::unique_ptr<FunctionCallAST> Parser::ParseFunctionCall(std::unique_ptr<Token> name)
{
    getNextToken(T_LPAREN);
    std::vector<std::unique_ptr<ASTBase>> args;
    while(current_token->getTokenType() != T_RPAREN)
    {
        if(auto arg = ParseExpression())
        {
            args.push_back(std::move(arg));
        }
        else
            return nullptr;

        if(current_token->getTokenType() == T_RPAREN)
            break;

        if(current_token->getTokenType() != T_COMMA)
        {
            LogError("PARSER: ParseFunctionCall(): Expected ')' or ',' in function call argument list\n");
            return nullptr;
        }
        
        getNextToken(T_COMMA);
    }

    getNextToken(T_RPAREN);

    auto call = std::make_unique<FunctionCallAST>(name->getValue(), std::move(args));
    return std::move(call);
}

std::unique_ptr<ASTBase> Parser::ParseIdentifier(bool atsign, bool ignore_assignment)
{
    auto id_copy = copyCurrentToken();

    getNextToken(T_IDENTIFIER);

    std::unique_ptr<ASTBase> expr;
    if(current_token->getTokenType() != T_LPAREN)
    {
        expr = std::make_unique<VariableAST>(id_copy->getValue());

        if(!ignore_assignment)
        {
            if(current_token->getTokenType() == T_EQUALS)
            {
                return ParseVariableAssignment(std::move(expr));
            }
            else if(current_token->getTokenType() == T_ADDEQ
                 || current_token->getTokenType() == T_SUBEQ
                 || current_token->getTokenType() == T_MULEQ
                 || current_token->getTokenType() == T_DIVEQ
                 || current_token->getTokenType() == T_MODEQ)
            {
                return ParseShorthandVariableAssignment(std::move(expr));
            }
        }

        return std::move(expr);
    }

    // It is a function call
    return ParseFunctionCall(std::move(id_copy));
}

std::unique_ptr<NumberAST> Parser::ParseNumber()
{
    auto token = copyCurrentToken();
    getNextToken(T_NUMBER);

    double num = std::stod(token->getValue());
    
    auto ast = std::make_unique<NumberAST>(0);
    ast->setValue(num);

    return std::move(ast);
}
std::unique_ptr<StringAST> Parser::ParseString()
{
    auto token = copyCurrentToken();
    getNextToken(T_STRING);

    return std::make_unique<StringAST>(token->getValue());
}

std::unique_ptr<ASTBase> Parser::ParseParenthesis()
{
    getNextToken(T_LPAREN);

    auto stm = ParsePrimary();
    if(!stm)
        return nullptr;

    stm = ParseBinaryOperation(0, std::move(stm));

    if(current_token->getTokenType() != T_RPAREN)
        return LogError("PARSER: ParseParenthesis(): Expected ')' at end of parenthesis expression\n");

    getNextToken(T_RPAREN);

    return std::move(stm);
}

std::unique_ptr<ASTBase> Parser::ParseBinaryOperation(int expr_precedence, std::unique_ptr<ASTBase> lhs)
{
    if(current_token->getTokenType() == T_EOF)
        return std::move(lhs);

    while(true)
    {
        int prec = -1;
        if(ParserPrecedenceMap.count(current_token->getValue()))
            prec = ParserPrecedenceMap[current_token->getValue()];

        if(prec < expr_precedence && !(current_token->getTokenType() == T_AND || current_token->getTokenType() == T_OR))
            return std::move(lhs);
        else if(current_token->getTokenType() == T_AND || current_token->getTokenType() == T_OR)
        {
            auto binop = copyCurrentToken();
            getNextTokenUnchecked();

            auto rhs = ParseExpression();
            return std::make_unique<BinaryOperationAST>(std::move(binop), std::move(lhs), std::move(rhs));
        }

        auto binop = copyCurrentToken();
        getNextTokenUnchecked();

        auto rhs = ParsePrimary();
        if(!rhs)
            return nullptr;
        
        int next_prec = -1;
        if(ParserPrecedenceMap.count(current_token->getValue()))
            next_prec = ParserPrecedenceMap[current_token->getValue()];

        if(prec < next_prec)
        {
            rhs = ParseBinaryOperation(prec + 1, std::move(rhs));
            if(!rhs)
                return nullptr;
        }

        lhs = std::make_unique<BinaryOperationAST>(std::move(binop), std::move(lhs), std::move(rhs));
        if(current_token->getTokenType() == T_EOF)
            return std::move(lhs);
    }
}

std::unique_ptr<VariableDefinitionAST> Parser::ParseVariableDefinition()
{
    getNextToken(T_LET);

    auto var_name = copyCurrentToken();
    getNextToken(T_IDENTIFIER);

    getNextToken(T_EQUALS);
    auto expr = ParseExpression();

    return std::make_unique<VariableDefinitionAST>(var_name->getValue(), std::move(expr));
}
std::unique_ptr<VariableAssignmentAST> Parser::ParseVariableAssignment(std::unique_ptr<ASTBase> expression)
{
    std::string name = "";
    switch(expression->type)
    {
        case AST_VAR: name = ((VariableAST*)expression.get())->getName();
        case AST_VARDEF: name = ((VariableDefinitionAST*)expression.get())->getName();
    }

    getNextToken(T_EQUALS);
    auto value = ParseExpression();

    return std::make_unique<VariableAssignmentAST>(name, std::move(value));
}
std::unique_ptr<VariableAssignmentAST> Parser::ParseShorthandVariableAssignment(std::unique_ptr<ASTBase> expression)
{
    std::string name = "";
    switch(expression->type)
    {
        case AST_VAR: name = ((VariableAST*)expression.get())->getName();
        case AST_VARDEF: name = ((VariableDefinitionAST*)expression.get())->getName();
    }

    auto token = copyCurrentToken();
    getNextTokenUnchecked();

    auto value = ParseExpression();

    std::unique_ptr<Token> symbol;
    switch(token->getTokenType())
    {
        case T_ADDEQ: {
            symbol = std::make_unique<Token>(T_ADD, "+", token->line, token->charpos);
            break;
        }
        case T_SUBEQ: {
            symbol = std::make_unique<Token>(T_SUB, "-", token->line, token->charpos);
            break;
        }
        case T_MULEQ: {
            symbol = std::make_unique<Token>(T_MUL, "*", token->line, token->charpos);
            break;
        }
        case T_DIVEQ: {
            symbol = std::make_unique<Token>(T_DIV, "/", token->line, token->charpos);
            break;
        }
        case T_MODEQ: {
            symbol = std::make_unique<Token>(T_MOD, "%", token->line, token->charpos);
            break;
        }
    }

    return std::make_unique<VariableAssignmentAST>(name, std::move(value), std::move(symbol));
}

std::unique_ptr<SequenceAST> Parser::ParseSequence()
{
    getNextToken(T_LARROW);

    std::vector<std::unique_ptr<FunctionCallAST>> calls;
    while(current_token->getTokenType() != T_RARROW)
    {
        auto name_token = copyCurrentToken();
        getNextToken(T_IDENTIFIER);

        if(current_token->getTokenType() != T_LPAREN)
        {
            LogError("PARSER: ParseSequence(): Only function calls are allowed in a sequence\n");
            return nullptr;
        }
        
        auto call = ParseFunctionCall(std::move(name_token));
        if(!call)
            return nullptr;
        calls.push_back(std::move(call));

        if(current_token->getTokenType() != T_RARROW)
            getNextToken(T_COMMA);
    }
    
    getNextToken(T_RARROW);

    return std::make_unique<SequenceAST>(std::move(calls));
}
std::unique_ptr<ASTBase> Parser::ParseDo()
{
    getNextToken(T_DO);

    auto val = ParseExpression();
    if(!val)
    {
        return nullptr;
    }
    if(val->type != AST_SEQUENCE && val->type != AST_VAR)
    {
        return LogError("PARSER: ParseDo(): Given AST is neither a sequence nor a variable");
    }

    std::vector<std::unique_ptr<ASTBase>> sequences;
    sequences.push_back(std::move(val));
    
    switch(current_token->getTokenType())
    {
        default: {
            return LogError("PARSER: ParseDo(): Only keywords `for` and `through` can be used after a sequence in a `do` statement\n");
        }

        case T_FOR: {
            getNextToken(T_FOR);
            auto for_times_ast = ParseExpression();

            return std::make_unique<DoForAST>(std::move(for_times_ast), std::move(sequences)); 
        }

        case T_THROUGH: {
            return LogError("PARSER: ParseDo(): Do-Through not implemented yet\n"); 
        }
    }
}

std::unique_ptr<ExternAST> Parser::ParseExtern()
{
    getNextToken(T_EXTERN);

    auto name = copyCurrentToken();
    getNextToken(T_IDENTIFIER);

    return std::make_unique<ExternAST>(name->getValue());
}

std::unique_ptr<NumberAST> Parser::ParseTrueFalse()
{
    auto tok = copyCurrentToken();
    getNextTokenUnchecked();

    if(tok->getTokenType() == T_TRUE)
        return std::make_unique<NumberAST>(1);
    else
        return std::make_unique<NumberAST>(0);
}
std::unique_ptr<IfAST> Parser::ParseIf()
{
    getNextToken(T_IF);
    getNextToken(T_LPAREN);
    
    auto expression = ParseExpression();

    getNextToken(T_RPAREN);

    std::vector<std::unique_ptr<ASTBase>> statements;
    if(current_token->getTokenType() != T_LBRACE)
    {
        auto stm = ParsePrimary();
        if(!stm)
            return nullptr;
        statements.push_back(std::move(stm));
    }
    else
    {
        getNextToken(T_LBRACE);
        while(current_token->getTokenType() != T_RBRACE)
        {
            auto stm = ParsePrimary();
            if(!stm)
                return nullptr;
            statements.push_back(std::move(stm));

            if(current_token->getTokenType() != T_RBRACE)
                getNextToken(T_NEXTLINE);
        }
        getNextToken(T_RBRACE);
    }
    
    return std::make_unique<IfAST>(std::move(expression), std::move(statements));
}
std::unique_ptr<ASTBase> Parser::ParseIfElse()
{
    bool has_else = false;
    std::vector<std::unique_ptr<IfAST>> if_statements;
    while(current_token->getTokenType() == T_IF)
    {
        auto ast = ParseIf();
        if(!ast)
            return nullptr;
        
        if_statements.push_back(std::move(ast));

        if(current_token->getTokenType() == T_ELSE)
        {
            getNextToken(T_ELSE);
            if(current_token->getTokenType() != T_IF)
            {
                has_else = true;
                break;
            }
        }
        else
        {
            break;
        }
    }

    std::vector<std::unique_ptr<ASTBase>> else_stms;
    if(has_else)
    {
        if(current_token->getTokenType() != T_LBRACE)
        {
            auto stm = ParsePrimary();
            if(!stm)
                return nullptr;
            else_stms.push_back(std::move(stm));
        }
        else
        {
            getNextToken(T_LBRACE);
            while(current_token->getTokenType() != T_RBRACE)
            {
                auto stm = ParsePrimary();
                if(!stm)
                    return nullptr;
                else_stms.push_back(std::move(stm));

                if(current_token->getTokenType() != T_RBRACE)
                    getNextToken(T_NEXTLINE);
            }
            getNextToken(T_RBRACE);
        }
    }

    return std::make_unique<IfElseAST>(std::move(if_statements), std::move(else_stms));
}

std::unique_ptr<MainAST> Parser::ParseMain(std::string const& program_name)
{
    getNextTokenUnchecked(true);
    parse_success = true;

    std::vector<std::unique_ptr<ASTBase>> statements;
    std::vector<std::unique_ptr<ExternAST>> externs;
    while(current_token->getTokenType() != T_EOF)
    {
        if(current_token->getTokenType() == T_EXTERN)
        {
            auto extern_ast = ParseExtern();
            externs.push_back(std::move(extern_ast));
            if(current_token->getTokenType() == T_NEXTLINE)
                getNextToken(T_NEXTLINE);
        }
        else
        {
            auto stm = ParsePrimary();
            if(current_token->getTokenType() == T_NEXTLINE)
                getNextToken(T_NEXTLINE);

            statements.push_back(std::move(stm));
        }
    }

    if(!parse_success)
    {
        LogError("PARSER: ParseMain(): Could not parse input code successfully\n");
        return nullptr;
    }
    
    return std::make_unique<MainAST>(std::move(statements), std::move(externs), program_name);
}

}