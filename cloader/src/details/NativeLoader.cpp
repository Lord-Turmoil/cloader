#include "cloader/details/NativeLoader.h"

#include "cloader/details/Config.h"

namespace
{

std::string ResolveMangledSymbolName(const char* className)
{
    std::string symbolName = CLOADER_SYMBOL_PREFIX;

    symbolName.push_back('_');
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

std::string ResolveSymbolName(const char* className)
{
    if (className && *className)
    {
        return ResolveMangledSymbolName(className);
    }

    return { CLOADER_SYMBOL_PREFIX };
}

} // namespace cloader::details
