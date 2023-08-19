#include <iostream>

#include "interpret.h"

#define IMPL_LANG_SYSLIB

namespace lang
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
            std::cout << static_cast<VariableNumberData*>(val)->getValue() << std::endl;
        }
        else if(val->getType() == VT_STRING)
        {
            std::cout << static_cast<VariableStringData*>(val)->getValue() << std::endl;
        }
        else if(val->getType() == VT_SEQUENCE)
        {
            std::cout << "<sequence>" << std::endl;
        }
        else if(val->getType() == VT_STRUCT)
        {
            std::cout << "<struct>" << std::endl;
        }

        return std::make_unique<VariableVoidData>();
    }

    static FCIType toStringFunction(FCIArguments args)
    {
        std::string retval = "<unknown>";
        auto* val = args.at("val");
        if(val->getType() == VT_NUMBER)
        {
            retval = std::to_string(((VariableNumberData*)val)->getValue());
            retval.erase ( retval.find_last_not_of('0') + 1, std::string::npos );
            retval.erase ( retval.find_last_not_of('.') + 1, std::string::npos );
        }
        else if(val->getType() == VT_STRING)
        {
            retval = ((VariableStringData*)val)->getValue();
        }
        else if(val->getType() == VT_SEQUENCE)
        {
            retval = "<sequence>";
        }
        else if(val->getType() == VT_STRUCT)
        {
            retval = "<struct>";
        }

        return std::make_unique<VariableStringData>(retval);
    }
    static FCIType toNumberFunction(FCIArguments args)
    {
        double retval = 0;
        auto* val = args.at("val");
        switch (val->getType())
        {
            case VT_NUMBER: retval = ((VariableNumberData*)val)->getValue(); break;
            case VT_STRING: retval = std::stod(((VariableStringData*)val)->getValue()); break;
            default: {
                std::cout << "toNumber(): Given value is of invalid type, could not convert to number" << std::endl;
            }
        }

        return std::make_unique<VariableNumberData>(retval);
    }
};
#endif

static void registerLibraries(Interpreter* interpreter)
{

#ifdef IMPL_LANG_SYSLIB
    interpreter->registerFunctionLibrary<Syslib>();
#endif

}

}
}