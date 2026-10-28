#include "cloader/details/NativeLoader.h"

#include "cloader/details/Config.h"

namespace
{

std::string ResolveMangledSymbolName(const char* className)
{
    std::string symbolName;

    for (const char* p = className; *p; ++p)
    {
        if (*p == ':')
        {
            symbolName.push_back('_');
        }
        else
        {
            symbolName.push_back(*p);
        }
    }

    return symbolName;
}

} // namespace

namespace cloader::details
{

std::string ResolveSymbolName(const char* prefix, const char* className)
{
    std::string symbolName = prefix;

    if (className && *className)
    {
        symbolName += "_";
        symbolName += ResolveMangledSymbolName(className);
    }

    return symbolName;
}

} // namespace cloader::details
