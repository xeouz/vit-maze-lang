#include <iostream>

#include "interpret.h"

#ifdef IMPL_LANG_DEFS
    #define ARG(name, type) {name, type}
    #define NUMBER xeouz::VT_NUMBER
    #define STRING xeouz::VT_STRING
    #define VOID xeouz::VT_VOID
    #define ANY xeouz::VT_ANY
    #define SEQUENCE xeouz::VT_SEQUENCE
    #define CREATE_NUMBER(value) xeouz::VariableNumberData::create(value)
    #define CREATE_STRING(value) xeouz::VariableStringData::create(value)
    #define CREATE_SEQUENCE(value) xeouz:VariableSequenceData::create(value)
    #define FUNCTION static xeouz::FCIType
    #define ARGUMENTS xeouz::FCIArguments

    #define LIBRARY_END()       }
    #define ADD_FUNCTION(funcname, ...)  useFunction(#funcname, &funcname, {.args = {__VA_ARGS__ }

    #define RETURNS(type) ,.ret_type = type}); 
    #define LIBRARY_BEGIN(libname)      \
                                public: \
                                    libname(): FCIFunctionLibraryBase(#libname) { \

#endif

namespace xeouz
{
namespace lib
{

#ifdef IMPL_LANG_SYSLIB
class Syslib: public FCIFunctionLibraryBase
{
public:
    Syslib(): FCIFunctionLibraryBase("sys")
    {
        useFunction("print", &Syslib::printFunction, {.args = {{"val", VT_ANY}}, .ret_type = VT_VOID});
        useFunction("toNumber", &Syslib::toNumberFunction, {.args = {{"val", VT_ANY}}, .ret_type = VT_NUMBER});
        useFunction("toString", &Syslib::toStringFunction, {.args = {{"val", VT_ANY}}, .ret_type = VT_STRING});
    }

    static FCIType printFunction(FCIArguments args)
    {
        auto* val = args.at("val");
        if(val->getType() == VT_NUMBER)
        {
            std::cout << val->getAsNumber()->getValue() << std::endl;
        }
        else if(val->getType() == VT_STRING)
        {
            std::cout << val->getAsString()->getValue() << std::endl;
        }
        else if(val->getType() == VT_SEQUENCE)
        {
            std::cout << "<sequence>" << std::endl;
        }
        else if(val->getType() == VT_STRUCT)
        {
            std::cout << "<struct>" << std::endl;
        }

        return VariableVoidData::create();
    }

    static FCIType toStringFunction(FCIArguments args)
    {
        std::string retval = "<unknown>";
        auto* val = args.at("val");
        if(val->getType() == VT_NUMBER)
        {
            retval = std::to_string(val->getAsNumber()->getValue());
            retval.erase ( retval.find_last_not_of('0') + 1, std::string::npos );
            retval.erase ( retval.find_last_not_of('.') + 1, std::string::npos );
        }
        else if(val->getType() == VT_STRING)
        {
            retval = val->getAsString()->getValue();
        }
        else if(val->getType() == VT_SEQUENCE)
        {
            retval = "<sequence>";
        }
        else if(val->getType() == VT_STRUCT)
        {
            retval = "<struct>";
        }

        return VariableStringData::create(retval);
    }
    static FCIType toNumberFunction(FCIArguments args)
    {
        double retval = 0;
        auto* val = args.at("val");
        switch (val->getType())
        {
            case VT_NUMBER: retval = val->getAsNumber()->getValue(); break;
            case VT_STRING: retval = std::stod(val->getAsString()->getValue()); break;
            default: {
                std::cout << "toNumber(): Given value is of invalid type, could not convert to number" << std::endl;
            }
        }

        return VariableNumberData::create(retval);
    }
};
#endif

static void registerLibraries(Interpreter* interpreter)
{

#ifdef IMPL_LANG_SYSLIB
    interpreter->registerFunctionLibrary<Syslib>();
#endif

}
static void registerLibraries(std::unique_ptr<Interpreter> const& interpreter)
{
    registerLibraries(interpreter.get());
}

}
}