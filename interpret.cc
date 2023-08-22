#include "interpret.h"

#include <iostream>
#include <type_traits>

namespace xeouz
{

///--- Variable Data ---///
VariableDataBase::VariableDataBase(int _type): type(_type)
{

}
int const VariableDataBase::getType() const
{
    return type;
}
VariableDataBase* VariableDataBase::copy() const
{
    return new VariableDataBase(type);
}
VariableDataBase* VariableDataBase::copyByType(VariableDataBase* val)
{
    switch(val->type)
    {
        case VT_NUMBER: return ((VariableNumberData*)val)->copy(); 
        case VT_STRING: return ((VariableStringData*)val)->copy(); 
        case VT_SEQUENCE: return ((VariableSequenceData*)val)->copy();
        default: return val->copy();
    }
}

VariableVoidData::VariableVoidData(): VariableDataBase(VT_VOID)
{

}
std::unique_ptr<VariableVoidData> VariableVoidData::create()
{
    return std::make_unique<VariableVoidData>();
}

VariableNumberData::VariableNumberData(double _value): value(_value), VariableDataBase(VT_NUMBER)
{

}
double const VariableNumberData::getValue() const
{
    return value;
}
void VariableNumberData::setValue(double _value)
{
    value = _value;
}
VariableDataBase* VariableNumberData::copy() const
{
    return new VariableNumberData(value);
}
std::unique_ptr<VariableNumberData> VariableNumberData::create(double value)
{
    return std::make_unique<VariableNumberData>(value);
}

VariableStringData::VariableStringData(std::string const& _value): value(_value), VariableDataBase(VT_STRING)
{

}
std::string const& VariableStringData::getValue() const
{
    return value;
}
void VariableStringData::setValue(std::string const& _value)
{
    value = _value;
}
VariableDataBase* VariableStringData::copy() const
{
    return new VariableStringData(value);
}
std::unique_ptr<VariableStringData> VariableStringData::create(std::string const& value)
{
    return std::make_unique<VariableStringData>(value);
}

VariableSequenceData::VariableSequenceData(std::vector<FunctionCallAST*> _calls): calls(std::move(_calls)), VariableDataBase(VT_SEQUENCE)
{
    
}
std::vector<FunctionCallAST*> const& VariableSequenceData::getValue() const
{
    return calls;
}
void VariableSequenceData::setValue(std::vector<FunctionCallAST*> _calls)
{
    calls = std::move(_calls);
}
VariableDataBase* VariableSequenceData::copy() const
{
    return new VariableSequenceData(calls);
}
std::unique_ptr<VariableSequenceData> VariableSequenceData::create(std::vector<FunctionCallAST*> value)
{
    return std::make_unique<VariableSequenceData>(value);
}
///--- Variable Data ---///

///--- Function Call Interface ---///
FCICallFunctionArguments::FCICallFunctionArguments()
{

}
FCICallFunctionArguments::FCICallFunctionArguments(std::vector<std::unique_ptr<VariableDataBase>> _args): args(std::move(_args))
{

}
std::vector<std::unique_ptr<VariableDataBase>> const& FCICallFunctionArguments::getArguments() const
{
    return args;
}
void FCICallFunctionArguments::addArgument(std::unique_ptr<VariableDataBase> value)
{
    args.push_back(std::move(value));
}
std::string FCICallFunctionArguments::generateSignature() const
{
    std::string sig = "";
    for(auto& arg: args)
    {
        sig += arg->getType();
        sig += " ";
    }
    return sig;
}

FCIFunction::FCIFunction(int _return_type, FCIFunctionPtr ptr, std::vector<std::pair<std::string, int>> _call_signature): return_type(_return_type)
{
    setCallSignature(_call_signature);
    setFunctionCallPtr(ptr);
}
void FCIFunction::setFunctionCallPtr(FCIFunctionPtr ptr)
{
    call_function = ptr;
}
void FCIFunction::setCallSignature(std::vector<std::pair<std::string, int>> const& call_sig)
{
    call_signature = call_sig;
}
std::unique_ptr<VariableDataBase> FCIFunction::call(std::unique_ptr<FCICallFunctionArguments> arguments)
{
    FCIArguments argument_list;
    
    if(call_signature.size() != arguments->getArguments().size())
    {
        std::cout << "FCIFunctionBase: call(): Argument list does match call signature" << std::endl;
        return nullptr;
    }

    for(int i=0; i<call_signature.size(); ++i)
    {
        auto const& arg_sig = call_signature.at(i);
        auto* arg = arguments->getArguments().at(i).get();

        if(arg->getType() != arg_sig.second && arg_sig.second != VT_ANY)
        {
            std::cout << "FCIFunctionBase: call(): Argument at " << i << " does not match call signature type" << std::endl;
            return nullptr;
        }

        argument_list.insert(std::make_pair(arg_sig.first, arg));
    }

    return call_function(std::move(argument_list));
}

FCIFunctionLibraryBase::FCIFunctionLibraryBase(std::string const& lib_name): name(lib_name)
{

}
std::string const& FCIFunctionLibraryBase::getLibraryName() const
{
    return name;
}
void FCIFunctionLibraryBase::setLibraryName(std::string const& lib_name)
{
    name = lib_name;
}
void FCIFunctionLibraryBase::useFunction(std::string const& name, FCIFunctionPtr ptr, FCIImplementableFunctionArguments const& args)
{
    auto func = std::make_unique<FCIFunction>(args.ret_type, ptr, args.args);
    lib.insert(std::make_pair(name, std::move(func)));
}
std::unordered_map<std::string, std::unique_ptr<FCIFunction>> FCIFunctionLibraryBase::moveLibrary()
{
    return std::move(lib);
}
///--- Function Call Interface ---///

///--- Interpreter ---///
Interpreter::Interpreter(std::unique_ptr<Parser> _parser): parser(std::move(_parser))
{

}

VariableDataBase* Interpreter::LogError(std::string const& str)
{
    std::cout << str << std::endl;
    return nullptr;
}
std::unique_ptr<VariableDataBase> Interpreter::LogErrorU(std::string const& str)
{
    std::cout << str << std::endl;
    return nullptr;
}

void Interpreter::defineVariable(std::string const& name, std::unique_ptr<VariableDataBase> data)
{
    if(data == nullptr)
        data = std::make_unique<VariableVoidData>();
    memory.insert(std::make_pair(name, std::move(data)));
}
bool Interpreter::isVariableDefined(std::string const& name)
{
    return memory.count(name);
}
VariableDataBase* const Interpreter::getVariableValue(std::string const& name)
{
    return memory.at(name).get();
}
void Interpreter::replaceVariableValue(std::string const& name, std::unique_ptr<VariableDataBase> new_value)
{
    memory[name] = std::move(new_value);
}
void Interpreter::changeVariableNumberValue(std::string const& name, double new_value)
{
    ((VariableNumberData*)memory.at(name).get())->setValue(new_value);
}
void Interpreter::changeVariableStringValue(std::string const& name, std::string const& new_value)
{
    ((VariableStringData*)memory.at(name).get())->setValue(new_value);
}

bool Interpreter::isFunctionDefined(std::string const& name)
{
    return functions.count(name);
}
FCIFunction* const Interpreter::getFunction(std::string const& name)
{
    return functions.at(name).get();
}
FCIType Interpreter::callFunction(std::string const& name, std::unique_ptr<FCICallFunctionArguments> args)
{
    auto* func = functions.at(name).get();
    return func->call(std::move(args));
}

VariableDataBase* const Interpreter::interpretPrimary(ASTBase* const ast)
{
    switch(ast->type)
    {
        default: {
            return LogError("INTERPRETER: interpretPrimary(): Unable to interpret invalid AST type `"+ast->toString()+"`");
        }
        case AST_VARASSIGN: return interpretVariableAssignment((VariableAssignmentAST* const)ast);
        case AST_VARDEF: return interpretVariableDefinition((VariableDefinitionAST* const)ast);

        case AST_IFELSE: return interpretIfElse((IfElseAST* const)ast);
        
        case AST_DOFOR: return interpretDoFor((DoForAST* const)ast);
        case AST_CALL: {
            interpretFunctionCall((FunctionCallAST* const)ast);
            return nullptr;
        }
        // case AST_DOTHROUGH: return interpretDoThrough((DoThroughAST* const)ast);
    }
}
std::unique_ptr<VariableDataBase> Interpreter::interpretExpression(ASTBase* const ast)
{
    switch(ast->type)
    {
        default: {
            return LogErrorU("INTERPRETER: interpretExpression(): Unable to interpret invalid AST type `"+ast->toString()+"`");
        }

        case AST_NUMBER: return interpretNumber((NumberAST* const)ast);
        case AST_STRING: return interpretString((StringAST* const)ast);

        case AST_CALL: return interpretFunctionCall((FunctionCallAST* const)ast);

        case AST_VAR: {
            auto* val = interpretVariable((VariableAST* const)ast);
            return std::unique_ptr<VariableDataBase>(VariableDataBase::copyByType(val));
        }

        case AST_SEQUENCE: return interpretSequence((SequenceAST* const)ast);
        
        case AST_BINOP: return interpretBinaryOperation((BinaryOperationAST* const)ast);
    }
}

VariableDataBase* const Interpreter::interpretVariable(VariableAST* const ast)
{
    if(!isVariableDefined(ast->getName()))
    {
        return LogError(std::string("INTERPRETER: interpretVariable(): Variable `"+ast->getName()+"` is not defined"));
    }

    return getVariableValue(ast->getName());
}
VariableDataBase* const Interpreter::interpretVariableDefinition(VariableDefinitionAST* const ast)
{
    if(isVariableDefined(ast->getName()))
    {
        return LogError(std::string("INTERPRETER: interpretVariableDefinition(): Variable `)"+ast->getName()+"` is already defined"));
    }

    auto val = interpretExpression(ast->getValue());
    if(val == nullptr)
    {
        return LogError(std::string("INTERPRETER: interpretVariableDefinition(): Assigned value is undefined in variable definition of `"+ast->getName()+"`"));
    }

    defineVariable(ast->getName(), std::move(val));

    return nullptr;
}
VariableDataBase* const Interpreter::interpretVariableAssignment(VariableAssignmentAST* const ast)
{
    if(!isVariableDefined(ast->getName()))
    {
        return LogError(std::string("INTERPRETER: interpretVariableAssignment(): Variable `)"+ast->getName()+"` is not defined"));
    }

    auto val = interpretExpression(ast->getValue());
    if(val == nullptr)
    {
        return LogError(std::string("INTERPRETER: interpretVariableDefinition(): Assigned value is undefined in variable assignment of `"+ast->getName()+"`"));
    }

    int var_type = getVariableValue(ast->getName())->getType();
    if(ast->isShorthand())
    {
        auto* op = ast->getShorthandOperator();
        auto op_val = useBinaryOperation(op, getVariableValue(ast->getName()), val.get());
        val = std::move(op_val);
    }

    if(val == nullptr)
    {
        return LogError(std::string("INTERPRETER: interpretVariableDefinition(): Assigned value is undefined in variable assignment of `"+ast->getName()+"`"));
    }
    
    if(val->getType() == var_type)
    {
        switch(val->getType())
        {
            case VT_NUMBER: changeVariableNumberValue(ast->getName(), ((VariableNumberData*)val.get())->getValue()); break;
            case VT_STRING: changeVariableStringValue(ast->getName(), ((VariableStringData*)val.get())->getValue()); break;
            default: replaceVariableValue(ast->getName(), std::move(val));
        }
    }
    else
    {
        replaceVariableValue(ast->getName(), std::move(val));
    }

    return nullptr;
}

std::unique_ptr<VariableNumberData> Interpreter::interpretNumber(NumberAST* const ast)
{
    return std::make_unique<VariableNumberData>(ast->getValue());
}
std::unique_ptr<VariableStringData> Interpreter::interpretString(StringAST* const ast)
{
    return std::make_unique<VariableStringData>(ast->getValue());
}

std::unique_ptr<VariableSequenceData> Interpreter::interpretSequence(SequenceAST* const ast)
{
    auto const& body = ast->getBody();

    std::vector<FunctionCallAST*> calls;
    for(auto&& call: body)
    {
        calls.push_back(call.get());
    }

    return std::make_unique<VariableSequenceData>(calls);
}
VariableDataBase* const Interpreter::interpretDoFor(DoForAST* const ast)
{
    auto for_times_base = interpretExpression(ast->getForTimes());
    if(!for_times_base)
    {
        return LogError("INTERPRETER: interpretDoFor(): Value specified in do-for is not of valid");
    }

    if(for_times_base->getType() != VT_NUMBER)
    {
        return LogError("INTERPRETER: interpretDoFor(): Value specified in do-for is not of type number");
    }

    auto* for_times = (VariableNumberData* const)for_times_base.get();
    auto const& sequences = ast->getSequences();
    for(int i=0; i<for_times->getValue(); ++i)
    {
        for(auto&& ast: sequences)
        {
            VariableSequenceData* seq;
            std::unique_ptr<VariableSequenceData> sequp;
            if(ast->type == AST_SEQUENCE)
            {
                sequp = interpretSequence((SequenceAST*)ast.get());
                seq = sequp.get();
            }
            else if(ast->type == AST_VAR)
            {
                auto* var = interpretVariable((VariableAST*)ast.get());
                if(!var)
                    return LogError("INTERPRETER: interpretDoFor(): Sequence variable given is invalid");
                else if(var->getType() != VT_SEQUENCE)
                    return LogError("INTERPRETER: interpretDoFor(): Sequence variable given is not of type <sequence>");
                
                seq = (VariableSequenceData*)var;
            }
            if(!seq)
            {
                return LogError("INTERPRETER: interpretDoFor(): Sequence given is invalid");
            }

            for(auto* stm: seq->getValue())
            {
                interpretFunctionCall(stm);
            }
        }
    }

    return nullptr;
}

std::unique_ptr<VariableDataBase> Interpreter::interpretFunctionCall(FunctionCallAST* const ast)
{
    if(!isFunctionDefined(ast->getName()))
    {
        return LogErrorU(std::string("INTERPRETER: interpretFunctionCall(): Function `")+ast->getName()+"` was not found");
    }

    int indx = 0;
    std::vector<FCIType> args;
    for(auto&& arg_ast: ast->getArguments())
    {
        auto val = interpretExpression(arg_ast.get());
        if(!val)
        {
            return LogErrorU(std::string("INTERPRETER: interpretFunctionCall(): In function call of `")+ast->getName()+"`, argument at index "+std::to_string(indx)+" is invalid"); 
        }
        args.push_back(std::move(val));
        indx++;
    }

    auto fci_args = std::make_unique<FCICallFunctionArguments>(std::move(args));

    return callFunction(ast->getName(), std::move(fci_args));
}

std::unique_ptr<VariableDataBase> Interpreter::useBinaryOperation(Token* op, VariableDataBase* lhs, VariableDataBase* rhs)
{
    if(!(lhs->getType() == VT_STRING || lhs->getType() == VT_NUMBER))
        return LogErrorU("INTERPRETER: useBinaryOperation(): LHS is neither a string nor a number");
    if(!(rhs->getType() == VT_STRING || rhs->getType() == VT_NUMBER))
        return LogErrorU("INTERPRETER: useBinaryOperation(): RHS is neither a string nor a number");

    if(lhs->getType() == rhs->getType() && lhs->getType() == VT_NUMBER) // If both are numbers
    {
        auto* nlhs = (VariableNumberData*)lhs;
        auto* nrhs = (VariableNumberData*)rhs;
        switch (op->getTokenType())
        {
            default: {
                return LogErrorU("INTERPRETER: useBinaryOperation(): Given token is unknown");
            }
            case T_ADD: return std::make_unique<VariableNumberData>(nlhs->getValue() + nrhs->getValue());
            case T_SUB: return std::make_unique<VariableNumberData>(nlhs->getValue() - nrhs->getValue());
            case T_MUL: return std::make_unique<VariableNumberData>(nlhs->getValue() * nrhs->getValue());
            case T_DIV: return std::make_unique<VariableNumberData>(nlhs->getValue() / nrhs->getValue());
            case T_MOD: return std::make_unique<VariableNumberData>((long)nlhs->getValue() % (long)nrhs->getValue());
            case T_DEQUAL: return std::make_unique<VariableNumberData>(nlhs->getValue() == nrhs->getValue());
            case T_NOTEQ: return std::make_unique<VariableNumberData>(nlhs->getValue() != nrhs->getValue());
            case T_LARROW: return std::make_unique<VariableNumberData>(nlhs->getValue() < nrhs->getValue());
            case T_RARROW: return std::make_unique<VariableNumberData>(nlhs->getValue() > nrhs->getValue());
            case T_LESSEQ: return std::make_unique<VariableNumberData>(nlhs->getValue() <= nrhs->getValue());
            case T_MOREEQ: return std::make_unique<VariableNumberData>(nlhs->getValue() >= nrhs->getValue());
        }
    }
    else if(lhs->getType() == rhs->getType() && lhs->getType() == VT_STRING) // If both are strings
    {
        auto* slhs = (VariableStringData*)lhs;
        auto* srhs = (VariableStringData*)rhs;

        switch (op->getTokenType())
        {
            default: {
                return LogErrorU(std::string("INTERPRETER: useBinaryOperation(): Cannot use token ")+op->toString()+" on a string");
            }
            case T_ADD: return std::make_unique<VariableStringData>(slhs->getValue() + srhs->getValue());
            case T_DEQUAL: return std::make_unique<VariableNumberData>(slhs->getValue() == srhs->getValue());
            case T_NOTEQ: return std::make_unique<VariableNumberData>(slhs->getValue() != srhs->getValue());
        }
    }
    else if(lhs->getType() != rhs->getType() && (lhs->getType() == VT_STRING || rhs->getType() == VT_STRING)) // If one of them is a string
    {
        auto* str = (lhs->getType()==VT_STRING)?(VariableStringData*)lhs : (VariableStringData*)rhs;
        auto* num = (lhs->getType()==VT_NUMBER)?(VariableNumberData*)lhs : (VariableNumberData*)rhs;
        int lhs_str = (lhs->getType()==VT_STRING);

        auto numstr = std::to_string(num->getValue());
        numstr.erase ( numstr.find_last_not_of('0') + 1, std::string::npos );
        numstr.erase ( numstr.find_last_not_of('.') + 1, std::string::npos );

        std::string string_data = "";
        if(lhs_str)
            string_data = str->getValue() + numstr;
        else
            string_data = numstr + str->getValue();

        switch(op->getTokenType())
        {
            default: {
                return LogErrorU(std::string("INTERPRETER: useBinaryOperation(): Cannot use token ")+op->toString()+" between a string and number");
            }

            case T_ADD: return std::make_unique<VariableStringData>(string_data);
        }
    }
    else
    {
        return LogErrorU("INTERPRETER: useBinaryOperation(): Binary operation is being used on invalid types");
    }
}
std::unique_ptr<VariableDataBase> Interpreter::interpretBinaryOperation(BinaryOperationAST* const ast)
{
    auto lhs = interpretExpression(ast->getLHS());
    auto rhs = interpretExpression(ast->getRHS());

    if(!lhs || !rhs)
    {
        return LogErrorU("INTERPRETER: interpretBinaryOperation(): Binary operation has invalid LHS or RHS");
    }

    return useBinaryOperation(ast->getOperator(), lhs.get(), rhs.get());
}

bool Interpreter::interpretIf(IfAST* const ast)
{
    auto expression = interpretExpression(ast->getExpression());
    if(expression->getType() != VT_NUMBER)
    {
        LogError("INTERPRETER: interpretIf(): Expression in if conditional is not of type number");
        return false;
    }
    
    auto* num = ((VariableNumberData*)expression.get());

    if(num->getValue() > 0)
    {
        for(auto&& stm: ast->getBody())
        {
            interpretPrimary(stm.get());
        }
        return true;
    }

    return false;
}
VariableDataBase* Interpreter::interpretIfElse(IfElseAST* const ast)
{
    bool did_run = false;
    for(auto&& ifstm: ast->getIfStatements()) // Run ifs
    {
        did_run = interpretIf(ifstm.get());
        if(did_run)
            break;
    }

    if(!did_run) // Run else body
    {
        for(auto&& stm: ast->getElseBody())
        {
            interpretPrimary(stm.get());
        }
    }

    return nullptr;
}

void Interpreter::interpretMain()
{
    success = true;
    auto ast = parser->ParseMain();
    if(!ast)
    {
        LogError("INTERPRETER: interpretMain(): Stopping program execution");
        return;
    }

    auto const& body = ast->getBody();
    for(auto&& stm: body)
    {
        interpretPrimary(stm.get());
        if(!success)
        {
            LogError("INTERPRETER: interpretMain(): Stopping program execution");
            break;
        }
    }
}

std::unique_ptr<Interpreter> Interpreter::create(std::unique_ptr<Parser> parser)
{
    return std::make_unique<Interpreter>(std::move(parser));
}
std::unique_ptr<Interpreter> Interpreter::create(std::string const& in_text)
{
    auto lexer = Lexer::create(in_text);
    auto parser = Parser::create(std::move(lexer));
    return create(std::move(parser));
}
///--- Interpreter ---///
}