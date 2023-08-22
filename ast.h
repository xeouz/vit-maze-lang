#pragma once

#include <memory>
#include <vector>

#include "lex.h"

namespace xeouz
{

enum ASTType
{
    AST_MAIN,

    AST_VAR,
    AST_VARDEF,
    AST_VARASSIGN,

    AST_SEQUENCE,
    AST_DOFOR,
    AST_DOTHROUGH,

    AST_NUMBER,
    AST_STRING,

    AST_CALL,
    AST_EXTERN,

    AST_BINOP,
    AST_IF,
    AST_IFELSE,

    AST_ARRAY,
};

///--- Base AST ---///
class ASTBase
{
    std::unique_ptr<Token> token;
public:
    int type;

    ASTBase(int ast_type, std::unique_ptr<Token> token);
    ASTBase(int ast_type, std::string const& token_value);

    static std::string const GetStringFromType(int ast_type);
    std::string const toString() const;

    virtual Token* const getToken() const;
    virtual void setToken(std::unique_ptr<Token> new_token);

    virtual ~ASTBase() { }
};
///--- Base AST ---///

///--- Main AST ---///
class ExternAST;

class MainAST: public ASTBase
{
    std::string program_name;
    std::vector<std::unique_ptr<ASTBase>> body;
    std::vector<std::unique_ptr<ExternAST>> external_functions;
public:
    MainAST(std::vector<std::unique_ptr<ASTBase>> body, std::vector<std::unique_ptr<ExternAST>> external_functions, std::string const& program_name = "main");

    std::string const& getProgramName() const;
    void setProgramName(std::string const& name);

    std::vector<std::unique_ptr<ASTBase>> const& getBody() const;
    void setBody(std::vector<std::unique_ptr<ASTBase>> body);

    void AddExternalFunction(std::unique_ptr<ExternAST> extern_func);
};
///--- Main AST ---///

///--- Variable AST ---///
class VariableAST: public ASTBase
{
    std::string name;
public:
    VariableAST(std::string const& name);

    std::string const& getName() const;
    void setName(std::string const& new_name);
};
///--- Variable AST ---///

///--- Variable Definition AST ---///
class VariableDefinitionAST: public ASTBase
{
    std::string name;
    std::unique_ptr<ASTBase> value;
public:
    VariableDefinitionAST(std::string const& name, std::unique_ptr<ASTBase> value);

    std::string const& getName() const;
    void setName(std::string const& new_name);

    ASTBase* const getValue() const;
};
///--- Variable Definition AST ---///

///--- Variable Assignment AST ---///
class VariableAssignmentAST: public ASTBase
{
    std::string name;
    std::unique_ptr<ASTBase> value;

    bool is_shorthand;
    std::unique_ptr<Token> shorthand_operator;
public:
    VariableAssignmentAST(std::string const& name, std::unique_ptr<ASTBase> value, std::unique_ptr<Token> shorthand_operator=nullptr);

    std::string const& getName() const;
    void setName(std::string const& new_name);

    bool const isShorthand() const;
    Token* const getShorthandOperator() const;
    std::unique_ptr<Token> moveShorthandOperator();
    void setShorthandOperator(std::unique_ptr<Token> shorthand_operator);

    ASTBase* const getValue() const;
};
///--- Variable Assignment AST ---///

///--- Function Call AST ---///
class FunctionCallAST: public ASTBase
{
    std::string name;
    std::vector<std::unique_ptr<ASTBase>> arguments;
public:
    FunctionCallAST(std::string const& name);
    FunctionCallAST(std::string const& name, std::vector<std::unique_ptr<ASTBase>> arguments);

    std::string const& getName() const;
    void setName(std::string const& new_name);

    std::vector<std::unique_ptr<ASTBase>> const& getArguments() const;
    void setArguments(std::vector<std::unique_ptr<ASTBase>> arguments);

    FunctionCallAST* copy() const;
};
///--- Function Call AST ---///

///--- Sequence AST ---///
class SequenceAST: public ASTBase
{
    std::vector<std::unique_ptr<FunctionCallAST>> body;
public:
    SequenceAST();
    SequenceAST(std::vector<std::unique_ptr<FunctionCallAST>> body);

    std::vector<std::unique_ptr<FunctionCallAST>> const& getBody() const;
    void setBody(std::vector<std::unique_ptr<FunctionCallAST>> body);
};
///--- Sequence AST ---///

///--- Do-For AST ---///
class DoForAST: public ASTBase
{
    std::vector<std::unique_ptr<ASTBase>> sequences;
    std::unique_ptr<ASTBase> for_times_ast;
public:
    DoForAST(std::unique_ptr<ASTBase> for_times_ast);
    DoForAST(std::unique_ptr<ASTBase> for_times_ast, std::vector<std::unique_ptr<ASTBase>> sequences);

    ASTBase* const getForTimes() const;
    void setForTimes(std::unique_ptr<ASTBase> for_times);

    std::vector<std::unique_ptr<ASTBase>> const& getSequences() const;
    void setSequences(std::vector<std::unique_ptr<ASTBase>> sequences);
};
///--- Do-For AST ---///

///--- Do-Through AST ---///
class DoThroughAST: public ASTBase
{
    std::vector<std::unique_ptr<ASTBase>> sequences;
    std::unique_ptr<ASTBase> through_ast;
public:
    DoThroughAST(std::unique_ptr<ASTBase> through_ast);
    DoThroughAST(std::unique_ptr<ASTBase> through_ast, std::vector<std::unique_ptr<ASTBase>> sequences);

    ASTBase* const getThrough() const;
    void setThrough(std::unique_ptr<ASTBase> through_ast);

    std::vector<std::unique_ptr<ASTBase>> const& getSequences() const;
    void setSequences(std::vector<std::unique_ptr<ASTBase>> sequences);
};
///--- Do-Through AST ---///

///--- Number AST ---///
class NumberAST: public ASTBase
{
    double value;
public:
    NumberAST(double value);

    double const getValue() const;
    void setValue(double value);
};
///--- Number AST ---///

///--- String AST ---///
class StringAST: public ASTBase
{
    std::string value;
public:
    StringAST(std::string const& value);

    std::string const& getValue() const;
    void setValue(std::string const& value);
};
///--- String AST ---///

///--- Extern AST ---///
class ExternAST: public ASTBase
{
    std::string name;
public:
    ExternAST(std::string const& name);

    std::string const& getName() const;
    void setName(std::string const& name);
};
///--- Extern AST ---///

///--- Binary Operation AST ---///
class BinaryOperationAST: public ASTBase
{
    std::unique_ptr<Token> op;
    std::unique_ptr<ASTBase> lhs, rhs;
public:
    BinaryOperationAST(std::unique_ptr<Token> op, std::unique_ptr<ASTBase> lhs, std::unique_ptr<ASTBase> rhs);

    Token* const getOperator() const;
    ASTBase* const getLHS() const;
    ASTBase* const getRHS() const;

    std::unique_ptr<ASTBase> moveLHS();
    std::unique_ptr<ASTBase> moveRHS();

    void setOperator(std::unique_ptr<Token> new_op);
    void setLHS(std::unique_ptr<ASTBase> lhs);
    void setRHS(std::unique_ptr<ASTBase> rhs);
};
///--- Binary Operation AST ---///

///--- If AST ---///
class IfAST: public ASTBase
{
    std::unique_ptr<ASTBase> expression;
    std::vector<std::unique_ptr<ASTBase>> statements;
public:
    IfAST(std::unique_ptr<ASTBase> expression, std::vector<std::unique_ptr<ASTBase>> statements);

    ASTBase* const getExpression() const;
    void setExpression(std::unique_ptr<ASTBase> new_expression);

    std::vector<std::unique_ptr<ASTBase>> const& getBody() const;
    void setBody(std::vector<std::unique_ptr<ASTBase>> new_body);
};
///--- If AST ---///

///--- If-Else AST ---///
class IfElseAST: public ASTBase
{
    std::vector<std::unique_ptr<IfAST>> if_statements;
    std::vector<std::unique_ptr<ASTBase>> else_statements;
public:
    IfElseAST(std::vector<std::unique_ptr<IfAST>> if_statements, std::vector<std::unique_ptr<ASTBase>> else_statements);

    std::vector<std::unique_ptr<IfAST>> const& getIfStatements() const;
    void setIfStatements(std::vector<std::unique_ptr<IfAST>> new_if_statements);

    std::vector<std::unique_ptr<ASTBase>> const& getElseBody() const;
    void setBody(std::vector<std::unique_ptr<ASTBase>> new_body);
};
///--- If-Else AST ---///

}