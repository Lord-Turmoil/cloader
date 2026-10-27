#include "cloader/details/Config.h"

#ifdef CLOADER_PLATFORM_UNIX

#include "cloader/details/NativeLoader.h"

#include "cloader/ClassLoader.h"
#include "cloader/Exception.h"
#include "cloader/details/Logging.h"

#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <dlfcn.h>

namespace
{

std::unordered_map<void*, std::string> sHandleToFilepath;

}

namespace cloader::details
{

void* LoadLibrary(const char* filepath)
{
    if (!filepath || !*filepath)
    {
        throw cloader::InvalidArgumentException("Path to the library cannot be null or empty");
    }

    dlerror(); // clear existing errors

    void* handle = dlopen(filepath, RTLD_LAZY | RTLD_LOCAL);
    if (!handle)
    {
        logging::LogError(std::format("Failed to load library '{}': {}", filepath, dlerror()));
        throw cloader::FileNotFoundException(filepath);
    }

    logging::LogDebug(std::format("Library loaded: '{}'", filepath));
    sHandleToFilepath[handle] = filepath;

    return handle;
}

void UnloadLibrary(void* handle)
{
    if (handle)
    {
        auto it = sHandleToFilepath.find(handle);
        if (it != sHandleToFilepath.end())
        {
            if (dlclose(handle) != 0)
            {
                logging::LogError(std::format("Failed to unload library '{}': {}", it->second, dlerror()));
            }
            else
            {
                logging::LogDebug(std::format("Library unloaded: '{}'", it->second));
            }
        }
        else
        {
            throw cloader::InvalidArgumentException("Invalid library handle");
        }
    }
}

void* LoadSymbolFromLibrary(void* handle, const char* className)
{
    std::string symbolName = ResolveSymbolName(className);

    logging::LogDebug(std::format("Resolving symbol '{}' for class '{}'", symbolName, className));
    void* symbol = dlsym(handle, symbolName.c_str());
    if (!symbol)
    {
        logging::LogError(
            std::format("Failed to resolve symbol '{}' for class '{}': {}", symbolName, className, dlerror()));
        throw cloader::ClassNotFoundException(className, symbolName.c_str());
    }

    return symbol;
}

} // namespace cloader::details

#endif
