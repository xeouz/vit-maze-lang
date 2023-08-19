#pragma once

#include "lex.h"
#include "parse.h"
#include "ast.h"

#include <map>
#include <memory>
#include <functional>

namespace lang
{

///--- Variable Data ---///
enum VariableDataType
{
    VT_VOID,
    VT_NUMBER,
    VT_STRING,
    VT_SEQUENCE,
    VT_ARRAY,
    VT_STRUCT,
    VT_ANY,
};

class VariableDataBase
{
    int type;
public:
    VariableDataBase(int type = VT_VOID);

    virtual ~VariableDataBase() {}

    int const getType() const;

    virtual VariableDataBase* copy() const;

    static VariableDataBase* copyByType(VariableDataBase* val);
};

class VariableVoidData: public VariableDataBase
{
public:
    VariableVoidData();
};

class VariableNumberData: public VariableDataBase
{
    double value;
public:
    VariableNumberData(double value);

    double const getValue() const;
    void setValue(double value);

    VariableDataBase* copy() const;
};

class VariableStringData: public VariableDataBase
{
    std::string value;
public:
    VariableStringData(std::string const& value);

    std::string const& getValue() const;
    void setValue(std::string const& value);

    VariableDataBase* copy() const;
};

class VariableSequenceData: public VariableDataBase
{
    std::vector<FunctionCallAST*> calls;
public:
    VariableSequenceData(std::vector<FunctionCallAST*> calls);

    std::vector<FunctionCallAST*> const& getValue() const;
    void setValue(std::vector<FunctionCallAST*> calls);

    VariableDataBase* copy() const;
};
///--- Variable Data ---///

///--- Function Call Interface ---///
class FCICallFunctionArguments
{
    std::vector<std::unique_ptr<VariableDataBase>> args;
public:
    FCICallFunctionArguments();
    FCICallFunctionArguments(std::vector<std::unique_ptr<VariableDataBase>> args);

    std::vector<std::unique_ptr<VariableDataBase>> const& getArguments() const;
    void addArgument(std::unique_ptr<VariableDataBase> value);

    std::string generateSignature() const;
};

struct FCIImplementableFunctionArguments
{
    std::vector<std::pair<std::string, int>> args;
    int ret_type;
};

typedef std::unique_ptr<VariableDataBase> FCIType;
typedef std::unordered_map<std::string, VariableDataBase*> FCIArguments;
typedef std::function<std::unique_ptr<VariableDataBase>(FCIArguments)> FCIFunctionPtr;

class FCIFunction
{
    int return_type;
    std::vector<std::pair<std::string, int>> call_signature;

    FCIFunctionPtr call_function;
public:
    FCIFunction(int return_type, FCIFunctionPtr ptr, std::vector<std::pair<std::string, int>> call_signature);
    void setFunctionCallPtr(FCIFunctionPtr ptr);
    void setCallSignature(std::vector<std::pair<std::string, int>> const& call_sig);
    std::unique_ptr<VariableDataBase> call(std::unique_ptr<FCICallFunctionArguments> arguments);
};

class FCIFunctionLibraryBase
{
    std::string name;
    std::unordered_map<std::string, std::unique_ptr<FCIFunction>> lib;
public:
    FCIFunctionLibraryBase(std::string const& lib_name);

    std::string const& getLibraryName() const;
    void setLibraryName(std::string const& lib_name);

    std::unordered_map<std::string, std::unique_ptr<FCIFunction>> moveLibrary();

    void useFunction(std::string const& name, FCIFunctionPtr ptr, FCIImplementableFunctionArguments const& args);
};
///--- Function Call Interface ---///

///--- Interpreter ---///
class Interpreter
{
    std::unique_ptr<Parser> parser;
    std::unordered_map<std::string, std::unique_ptr<VariableDataBase>> memory;
    std::unordered_map<std::string, std::unique_ptr<FCIFunction>> functions;

    std::unique_ptr<VariableDataBase> useBinaryOperation(Token* op, VariableDataBase* lhs, VariableDataBase* rhs);
    bool success;
public:
    Interpreter(std::unique_ptr<Parser> parser);

    VariableDataBase* LogError(std::string const& str);
    std::unique_ptr<VariableDataBase> LogErrorU(std::string const& str);

    void defineVariable(std::string const& name, std::unique_ptr<VariableDataBase> data = nullptr);
    bool isVariableDefined(std::string const& name);
    VariableDataBase* const getVariableValue(std::string const& name);
    void replaceVariableValue(std::string const& name, std::unique_ptr<VariableDataBase> new_value);
    void changeVariableNumberValue(std::string const& name, double new_value);
    void changeVariableStringValue(std::string const& name, std::string const& new_value);

    bool isFunctionDefined(std::string const& name);
    FCIFunction* const getFunction(std::string const& name);
    FCIType callFunction(std::string const& name, std::unique_ptr<FCICallFunctionArguments> args);

    VariableDataBase* const interpretPrimary(ASTBase* const ast);
    std::unique_ptr<VariableDataBase> interpretExpression(ASTBase* const ast);

    VariableDataBase* const interpretVariable(VariableAST* const ast);
    VariableDataBase* const interpretVariableDefinition(VariableDefinitionAST* const ast);
    VariableDataBase* const interpretVariableAssignment(VariableAssignmentAST* const ast);
    
    std::unique_ptr<VariableNumberData> interpretNumber(NumberAST* const ast);
    std::unique_ptr<VariableStringData> interpretString(StringAST* const ast);

    std::unique_ptr<VariableSequenceData> interpretSequence(SequenceAST* const ast);
    VariableDataBase* const interpretDoFor(DoForAST* const ast);
    // VariableDataBase* const interpretDoThrough(DoThroughAST* const ast);

    std::unique_ptr<VariableDataBase> interpretFunctionCall(FunctionCallAST* const ast);
    void interpretExtern(ExternAST* const ast);

    std::unique_ptr<VariableDataBase> interpretBinaryOperation(BinaryOperationAST* const ast);

    void interpretMain();

    template <typename T>
    void registerFunctionLibrary()
    {
        static_assert(std::is_base_of<FCIFunctionLibraryBase, T>::value, "INTERPRETE: registerFunctionLibrary(): Given type is not a child of function library");

        T* libt = new T();

        auto lib = (FCIFunctionLibraryBase*)libt;
        auto funcs = lib->moveLibrary();

        for(auto&& it : funcs)
        {
            auto const& name = it.first;
            auto func = std::move(funcs[name]);

            std::pair<std::string, std::unique_ptr<FCIFunction>> p = std::make_pair(name, std::move(func));
            functions.insert(std::move(p));
        }

        delete libt;
    }
};
///--- Interpreter ---///

}