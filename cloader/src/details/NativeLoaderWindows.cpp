#include "cloader/details/Config.h"

#ifdef CLOADER_PLATFORM_WINDOWS

#include "details/NativeLoader.h"

#include "cloader/Exception.h"
#include "cloader/details/Logging.h"

#include <format>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

namespace
{

std::string GetFilepathForHandle(void* handle)
{
    char buffer[MAX_PATH];
    GetModuleFileName(static_cast<HMODULE>(handle), buffer, MAX_PATH);
    return std::string(buffer);
}

} // namespace

namespace cloader::details
{

void* LoadLibraryFromFile(const char* filepath)
{
    if (!filepath || !*filepath)
    {
        throw cloader::InvalidArgumentException("Path to the library cannot be null or empty");
    }

    void* handle = LoadLibrary(filepath);
    if (!handle)
    {
        logging::LogError(std::format("Failed to load library '{}': {}", filepath, GetLastError()));
        throw cloader::FileNotFoundException(filepath);
    }

    logging::LogDebug(std::format("Library loaded: '{}'", filepath));

    return handle;
}

void UnloadLibrary(void* handle) noexcept
{
    if (handle)
    {
        std::string filepath = GetFilepathForHandle(handle);
        if (!FreeLibrary(static_cast<HMODULE>(handle)))
        {
            logging::LogError(std::format("Failed to unload library '{}': {}", filepath, GetLastError()));
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
    void* symbol = GetProcAddress(static_cast<HMODULE>(handle), symbolName);
    if (!symbol)
    {
        std::string filepath = GetFilepathForHandle(handle);
        logging::LogWarning(
            std::format("Failed to resolve symbol '{}' in library '{}': {}", symbolName, filepath, GetLastError()));
    }
    return symbol;
}

} // namespace cloader::details

#endif
