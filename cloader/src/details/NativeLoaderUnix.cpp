#include "cloader/details/Config.h"

#ifdef CLOADER_PLATFORM_UNIX

#include "details/NativeLoader.h"

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

void SetFilepathForHandle(void* handle, const std::string& filepath)
{
    sHandleToFilepath[handle] = filepath;
}

const std::string& GetFilepathForHandle(void* handle)
{
    auto it = sHandleToFilepath.find(handle);
    if (it == sHandleToFilepath.end())
    {
        throw cloader::InvalidArgumentException("Invalid library handle");
    }
    return it->second;
}

} // namespace

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
    SetFilepathForHandle(handle, filepath);

    logging::LogDebug(std::format("Library loaded: '{}'", filepath));

    return handle;
}

void UnloadLibrary(void* handle) noexcept
{
    if (handle)
    {
        std::string filepath = GetFilepathForHandle(handle);
        if (dlclose(handle) != 0)
        {
            logging::LogError(std::format("Failed to unload library '{}': {}", filepath, dlerror()));
        }
        else
        {
            logging::LogDebug(std::format("Library unloaded: '{}'", filepath));
        }
    }
}

void* LoadSymbolFromLibrary(void* handle, const char* symbolName)
{
    if (!handle)
    {
        throw cloader::InvalidArgumentException("Library handle cannot be null");
    }
    if (!symbolName || !*symbolName)
    {
        throw cloader::InvalidArgumentException("Symbol name cannot be null or empty");
    }

    logging::LogDebug(std::format("Resolving symbol '{}'", symbolName));
    void* symbol = dlsym(handle, symbolName);
    if (!symbol)
    {
        std::string filepath = GetFilepathForHandle(handle);
        logging::LogWarning(
            std::format("Failed to resolve symbol '{}' in library '{}': {}", symbolName, filepath, dlerror()));
    }
    return symbol;
}

} // namespace cloader::details

#endif
