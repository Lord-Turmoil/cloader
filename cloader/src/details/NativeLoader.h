#pragma once

#include <string>

namespace cloader::details
{

void* LoadLibrary(const char* filepath);
void UnloadLibrary(void* handle);

std::string ResolveSymbolName(const char* prefix, const char* className);
void* LoadSymbolFromLibrary(void* handle, const char* className);

} // namespace cloader::details
