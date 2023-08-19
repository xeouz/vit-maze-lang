#include "ast.h"

namespace lang
{

///--- Base AST ---///
ASTBase::ASTBase(int ast_type, std::unique_ptr<Token> _token): type(ast_type), token(std::move(_token))
{

}
ASTBase::ASTBase(int ast_type, std::string const& token_value): type(ast_type)
{
    token = std::make_unique<Token>(T_IDENTIFIER, token_value);
}

Token* const ASTBase::getToken() const
{
    return token.get();
}
void ASTBase::setToken(std::unique_ptr<Token> new_token)
{
    token = std::move(new_token);
}

std::string const ASTBase::GetStringFromType(int ast_type)
{
    switch(ast_type)
    {
    case AST_MAIN: return "MAIN";

    case AST_VAR: return "VAR";
    case AST_VARDEF: return "VARDEF";
    case AST_VARASSIGN: return "VARASSIGN";

    case AST_SEQUENCE: return "SEQUENCE";
    case AST_DOFOR: return "DOFOR";
    case AST_DOTHROUGH: return "DOTHROUGH";

    case AST_NUMBER: return "NUMBER";
    case AST_STRING: return "STRING";

    case AST_CALL: return "CALL";
    case AST_EXTERN: return "EXTERN";

    case AST_BINOP: return "BINOP";
    case AST_IF: return "IF";
    case AST_IFELSE: return "IFELSE";

    case AST_ARRAY: return "ARRAY";

    default: return "<INVALID>";
    }
}
std::string const ASTBase::toString() const
{
    return GetStringFromType(type);
}
///--- Base AST ---///

///--- Main AST ---///
MainAST::MainAST(std::vector<std::unique_ptr<ASTBase>> _body, std::vector<std::unique_ptr<ExternAST>> _external_functions, std::string const& _program_name)
: body(std::move(_body)), external_functions(std::move(_external_functions)), program_name(_program_name), ASTBase(AST_MAIN, _program_name)
{

}

std::string const& MainAST::getProgramName() const
{
    return program_name;
}
void MainAST::setProgramName(std::string const& name)
{
    program_name = name;
}

std::vector<std::unique_ptr<ASTBase>> const& MainAST::getBody() const
{
    return body;
}
void MainAST::setBody(std::vector<std::unique_ptr<ASTBase>> _body)
{
    body = std::move(_body);
}

void MainAST::AddExternalFunction(std::unique_ptr<ExternAST> extern_func)
{
    external_functions.push_back(std::move(extern_func));
}
///--- Main AST ---///

///--- Variable AST ---///
VariableAST::VariableAST(std::string const& _name): name(_name), ASTBase(AST_VAR, _name)
{

}

std::string const& VariableAST::getName() const
{
    return name;
}
void VariableAST::setName(std::string const& new_name)
{
    name = new_name;
}
///--- Variable AST ---///

///--- Variable Definition AST ---///
VariableDefinitionAST::VariableDefinitionAST(std::string const& _name, std::unique_ptr<ASTBase> _value)
: name(_name), value(std::move(_value)), ASTBase(AST_VARDEF, _name)
{

}

std::string const& VariableDefinitionAST::getName() const
{
    return name;
}
void VariableDefinitionAST::setName(std::string const& new_name)
{
    name = new_name;
}

ASTBase* const VariableDefinitionAST::getValue() const
{
    return value.get();
}
///--- Variable Definition AST ---///

///--- Variable Assignment AST ---///
VariableAssignmentAST::VariableAssignmentAST(std::string const& _name, std::unique_ptr<ASTBase> _value, std::unique_ptr<Token> _shorthand_operator)
: name(_name), value(std::move(_value)), ASTBase(AST_VARASSIGN, _name), is_shorthand(false)
{
    setShorthandOperator(std::move(_shorthand_operator));
}

std::string const& VariableAssignmentAST::getName() const
{
    return name;
}
void VariableAssignmentAST::setName(std::string const& new_name)
{
    name = new_name;
}

bool const VariableAssignmentAST::isShorthand() const
{
    return is_shorthand;
}
std::unique_ptr<Token> VariableAssignmentAST::moveShorthandOperator()
{
    return std::move(shorthand_operator);
}
Token* const VariableAssignmentAST::getShorthandOperator() const
{
    return shorthand_operator.get();
}
void VariableAssignmentAST::setShorthandOperator(std::unique_ptr<Token> _shorthand_operator)
{
    if(_shorthand_operator)
    {
        is_shorthand = true;
        shorthand_operator = std::move(_shorthand_operator);
    }
    else
    {
        is_shorthand = false;
    }
}

ASTBase* const VariableAssignmentAST::getValue() const
{
    return value.get();
}
///--- Variable Assignment AST ---///

///--- Function Call AST ---///
FunctionCallAST::FunctionCallAST(std::string const& _name)
: name(_name), ASTBase(AST_CALL, _name)
{

}
FunctionCallAST::FunctionCallAST(std::string const& _name, std::vector<std::unique_ptr<ASTBase>> _arguments)
: name(_name), arguments(std::move(_arguments)), ASTBase(AST_CALL, _name)
{

}

std::string const& FunctionCallAST::getName() const
{
    return name;
}
void FunctionCallAST::setName(std::string const& new_name)
{
    name = new_name;
}

std::vector<std::unique_ptr<ASTBase>> const& FunctionCallAST::getArguments() const
{
    return arguments;
}
void FunctionCallAST::setArguments(std::vector<std::unique_ptr<ASTBase>> arguments)
{
    arguments = std::move(arguments);
}
///--- Function Call AST ---///

///--- Sequence AST ---///
SequenceAST::SequenceAST()
: ASTBase(AST_SEQUENCE, "")
{

}
SequenceAST::SequenceAST(std::vector<std::unique_ptr<FunctionCallAST>> _body)
: body(std::move(_body)), ASTBase(AST_SEQUENCE, "")
{

}

std::vector<std::unique_ptr<FunctionCallAST>> const& SequenceAST::getBody() const
{
    return body;
}
void SequenceAST::setBody(std::vector<std::unique_ptr<FunctionCallAST>> _body)
{
    body = std::move(_body);
}
///--- Sequence AST ---///

///--- Do-For AST ---///
DoForAST::DoForAST(std::unique_ptr<ASTBase> _for_times_ast)
: for_times_ast(std::move(_for_times_ast)), ASTBase(AST_DOFOR, "")
{

}
DoForAST::DoForAST(std::unique_ptr<ASTBase> _for_times_ast, std::vector<std::unique_ptr<ASTBase>> _sequences)
: for_times_ast(std::move(_for_times_ast)), sequences(std::move(_sequences)), ASTBase(AST_DOFOR, "")
{

}

ASTBase* const DoForAST::getForTimes() const
{
    return for_times_ast.get();
}
void DoForAST::setForTimes(std::unique_ptr<ASTBase> _for_times)
{
    for_times_ast = std::move(_for_times);
}

std::vector<std::unique_ptr<ASTBase>> const& DoForAST::getSequences() const
{
    return sequences;
}
void DoForAST::setSequences(std::vector<std::unique_ptr<ASTBase>> _sequences)
{
    sequences = std::move(_sequences);
}
///--- Do-For AST ---///

///--- Do-Through AST ---///
DoThroughAST::DoThroughAST(std::unique_ptr<ASTBase> _through_ast)
: through_ast(std::move(_through_ast)), ASTBase(AST_DOTHROUGH, "")
{

}
DoThroughAST::DoThroughAST(std::unique_ptr<ASTBase> _through_ast, std::vector<std::unique_ptr<ASTBase>> _sequences)
: through_ast(std::move(_through_ast)), sequences(std::move(_sequences)), ASTBase(AST_DOTHROUGH, "")
{

}

ASTBase* const DoThroughAST::getThrough() const
{
    return through_ast.get();
}
void DoThroughAST::setThrough(std::unique_ptr<ASTBase> _through_ast)
{
    through_ast = std::move(_through_ast);
}

std::vector<std::unique_ptr<ASTBase>> const& DoThroughAST::getSequences() const
{
    return sequences;
}
void DoThroughAST::setSequences(std::vector<std::unique_ptr<ASTBase>> _sequences)
{
    sequences = std::move(_sequences);
}
///--- Do-Through AST ---///

///--- Number AST ---///
NumberAST::NumberAST(double _value): value(_value), ASTBase(AST_NUMBER, "")
{

}

double const NumberAST::getValue() const
{
    return value;
}
void NumberAST::setValue(double _value)
{
    value = _value;
}
///--- Number AST ---///

///--- String AST ---///
StringAST::StringAST(std::string const& _value): value(_value), ASTBase(AST_STRING, "")
{

}

std::string const& StringAST::getValue() const
{
    return value;
}
void StringAST::setValue(std::string const& _value)
{
    value = _value;
}
///--- String AST ---///

///--- Extern AST ---///
ExternAST::ExternAST(std::string const& _name): name(_name), ASTBase(AST_EXTERN, "")
{

}

std::string const& ExternAST::getName() const
{
    return name;
}
void ExternAST::setName(std::string const& _name)
{
    name = _name;
}
///--- Extern AST ---///

///--- Binary Operation AST ---///
BinaryOperationAST::BinaryOperationAST(std::unique_ptr<Token> _op, std::unique_ptr<ASTBase> _lhs, std::unique_ptr<ASTBase> _rhs)
: op(std::move(_op)), lhs(std::move(_lhs)), rhs(std::move(_rhs)), ASTBase(AST_BINOP, "")
{

}

Token* const BinaryOperationAST::getOperator() const
{
    return op.get();
}
ASTBase* const BinaryOperationAST::getLHS() const
{
    return lhs.get();
}
ASTBase* const BinaryOperationAST::getRHS() const
{
    return rhs.get();
}

std::unique_ptr<ASTBase> BinaryOperationAST::moveLHS()
{
    return std::move(lhs);
}
std::unique_ptr<ASTBase> BinaryOperationAST::moveRHS()
{
    return std::move(rhs);
}

void BinaryOperationAST::setOperator(std::unique_ptr<Token> new_op)
{
    op = std::move(new_op);
}
void BinaryOperationAST::setLHS(std::unique_ptr<ASTBase> _lhs)
{
    lhs = std::move(_lhs);
}
void BinaryOperationAST::setRHS(std::unique_ptr<ASTBase> _rhs)
{
    rhs = std::move(_rhs);
}
///--- Binary Operation AST ---///

///--- If AST ---///
IfAST::IfAST(std::unique_ptr<ASTBase> _expression, std::vector<std::unique_ptr<ASTBase>> _statements)
: expression(std::move(_expression)), statements(std::move(_statements)), ASTBase(AST_IF, "if")
{

}

ASTBase* const IfAST::getExpression() const
{
    return expression.get();
}
void IfAST::setExpression(std::unique_ptr<ASTBase> new_expression)
{
    expression = std::move(new_expression);
}

std::vector<std::unique_ptr<ASTBase>> const& IfAST::getBody() const
{
    return statements;
}
void IfAST::setBody(std::vector<std::unique_ptr<ASTBase>> new_body)
{
    statements = std::move(new_body);
}
///--- If AST ---///

///--- If-Else AST ---///
IfElseAST::IfElseAST(std::vector<std::unique_ptr<IfAST>> _if_statements, std::vector<std::unique_ptr<ASTBase>> _else_statements)
: if_statements(std::move(_if_statements)), else_statements(std::move(_else_statements)), ASTBase(AST_IFELSE, "")
{

}

std::vector<std::unique_ptr<IfAST>> const& IfElseAST::getIfStatements() const
{
    return if_statements;
}
void IfElseAST::setIfStatements(std::vector<std::unique_ptr<IfAST>> new_if_statements)
{
    if_statements = std::move(new_if_statements);
}

std::vector<std::unique_ptr<ASTBase>> const& IfElseAST::getElseBody() const
{
    return else_statements;
}
void IfElseAST::setBody(std::vector<std::unique_ptr<ASTBase>> new_body)
{
    else_statements = std::move(new_body);
}
///--- If-Else AST ---///
}