#pragma once

#include <string>

namespace cloader::details
{

/**
 * @brief Load a dynamic library from the specified file path.
 * @param filepath The path to the dynamic library file.
 * @return A handle to the loaded library.
 * @throws InvalidArgumentException If the filepath is null or empty.
 * @throws FileNotFoundException If the dynamic library file is not found.
 */
void* LoadLibrary(const char* filepath);

/**
 * @brief Unload a dynamic library given its handle.
 * @param handle The handle to the loaded library.
 */
void UnloadLibrary(void* handle) noexcept;

/**
 * @brief Get the name of the symbol.
 * @details The class name should be fully qualified, e.g. "Namespace::ClassName". The resolved symbol name will be the
 * prefix plus the class name, where "::" is replaced with "__".
 * @param prefix The prefix to use for the symbol.
 * @param className The name of the class.
 * @return The resolved name of the symbol. If className is null or empty, the symbol name will be the prefix only.
 */
std::string ResolveSymbolName(const char* prefix, const char* className);

/**
 * @brief Load a symbol from a dynamic library given its handle and name.
 * @param handle The handle to the loaded library.
 * @param symbolName The name of the symbol to load.
 * @return A pointer to the symbol, or nullptr if the symbol is not found.
 * @throws InvalidArgumentException If the handle or symbolName is null or empty.
 */
void* LoadSymbolFromLibrary(void* handle, const char* symbolName);

} // namespace cloader::details
