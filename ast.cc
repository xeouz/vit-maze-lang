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
///--- Base AST ---///

///--- Main AST ---///
MainAST::MainAST(std::vector<std::unique_ptr<ASTBase>> _body, std::string const& _program_name = "main")
: body(std::move(_body)), program_name(_program_name), ASTBase(AST_MAIN, _program_name)
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
///--- Variable Definition AST ---///

///--- Variable Assignment AST ---///
VariableAssignmentAST::VariableAssignmentAST(std::string const& _name, std::unique_ptr<ASTBase> _value)
: name(_name), value(std::move(_value)), ASTBase(AST_VARASSIGN, _name)
{

}

std::string const& VariableAssignmentAST::getName() const
{
    return name;
}
void VariableAssignmentAST::setName(std::string const& new_name)
{
    name = new_name;
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
DoForAST::DoForAST(std::unique_ptr<ASTBase> _for_times_ast, std::vector<std::unique_ptr<SequenceAST>> _sequences)
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

std::vector<std::unique_ptr<SequenceAST>> const& DoForAST::getSequences() const
{
    return sequences;
}
void DoForAST::setSequences(std::vector<std::unique_ptr<SequenceAST>> _sequences)
{
    sequences = std::move(_sequences);
}
///--- Do-For AST ---///

///--- Do-Through AST ---///
DoThroughAST::DoThroughAST(std::unique_ptr<ASTBase> _through_ast)
: through_ast(std::move(_through_ast)), ASTBase(AST_DOTHROUGH, "")
{

}
DoThroughAST::DoThroughAST(std::unique_ptr<ASTBase> _through_ast, std::vector<std::unique_ptr<SequenceAST>> _sequences)
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

std::vector<std::unique_ptr<SequenceAST>> const& DoThroughAST::getSequences() const
{
    return sequences;
}
void DoThroughAST::setSequences(std::vector<std::unique_ptr<SequenceAST>> _sequences)
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

}