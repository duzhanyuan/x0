/*
 * This file is part of the x0 web server project and is released under AGPL-3.
 * http://www.xzero.io/
 *
 * (c) 2009-2014 Christian Parpart <trapni@gmail.com>
 */

#include <x0/flow/ir/IRProgram.h>
#include <x0/flow/ir/IRHandler.h>
#include <assert.h>

namespace x0 {

using namespace FlowVM;

IRProgram::IRProgram() :
    imports_(),
    numbers_(),
    strings_(),
    ipaddrs_(),
    cidrs_(),
    regexps_(),
    builtinFunctions_(),
    builtinHandlers_(),
    handlers_()
{
}

IRProgram::~IRProgram()
{
    for (auto& value: numbers_) delete value;
    for (auto& value: strings_) delete value;
    for (auto& value: ipaddrs_) delete value;
    for (auto& value: cidrs_) delete value;
    for (auto& value: regexps_) delete value;
    for (auto& value: handlers_) delete value;
}

void IRProgram::dump()
{
    printf("; IRProgram\n");

    for (auto handler: handlers_)
        handler->dump();
}

template<typename T, typename U>
T* IRProgram::get(std::vector<T*>& table, const U& literal)
{
    for (size_t i = 0, e = table.size(); i != e; ++i)
        if (table[i]->get() == literal)
            return table[i];

    T* value = new T(table.size(), literal);
    table.push_back(value);
    return value;
}

template ConstantInt* IRProgram::get<ConstantInt, int64_t>(std::vector<ConstantInt*>&, const int64_t&);
template ConstantString* IRProgram::get<ConstantString, std::string>(std::vector<ConstantString*>&, const std::string&);
template ConstantIP* IRProgram::get<ConstantIP, IPAddress>(std::vector<ConstantIP*>&, const IPAddress&);
template ConstantCidr* IRProgram::get<ConstantCidr, Cidr>(std::vector<ConstantCidr*>&, const Cidr&);
template ConstantRegExp* IRProgram::get<ConstantRegExp, RegExp>(std::vector<ConstantRegExp*>&, const RegExp&);
template IRBuiltinHandler* IRProgram::get<IRBuiltinHandler, Signature>(std::vector<IRBuiltinHandler*>&, const Signature&);
template IRBuiltinFunction* IRProgram::get<IRBuiltinFunction, Signature>(std::vector<IRBuiltinFunction*>&, const Signature&);

} // namespace x0
