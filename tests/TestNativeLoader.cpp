#include <cloader/cloader.h>

#include <cloader/details/Config.h>

#include <doctest/doctest.h>

// The native loader is internal to the library (it lives under cloader/src and
// is not installed), but testing it directly reaches error paths the public
// ClassLoader API cannot: foreign handles, null symbols and so on.
#ifdef CLOADER_PLATFORM_UNIX

#include <details/NativeLoader.h>

#include <dlfcn.h>

TEST_CASE("ResolveSymbolName keeps the prefix when the class name is null or empty")
{
    CHECK(cloader::details::ResolveSymbolName("P", nullptr) == "P");
    CHECK(cloader::details::ResolveSymbolName("P", "") == "P");
}

TEST_CASE("ResolveSymbolName mangles namespace separators")
{
    CHECK(cloader::details::ResolveSymbolName("P", "a::B") == "P_a__B");
    CHECK(cloader::details::ResolveSymbolName("P", "::x") == "P___x");
    CHECK(cloader::details::ResolveSymbolName("P", "x::") == "P_x__");
}

TEST_CASE("LoadLibrary rejects null or empty paths")
{
    CHECK_THROWS_WITH(cloader::details::LoadLibraryFromFile(nullptr), "Path to the library cannot be null or empty");
    CHECK_THROWS_WITH(cloader::details::LoadLibraryFromFile(""), "Path to the library cannot be null or empty");
}

TEST_CASE("LoadSymbolFromLibrary rejects null handles and symbol names")
{
    CHECK_THROWS_WITH(cloader::details::LoadSymbolFromLibrary(nullptr, "symbol"), "Library handle cannot be null");

    void* handle = dlopen(nullptr, RTLD_LAZY | RTLD_LOCAL);
    REQUIRE(handle != nullptr);

    CHECK_THROWS_WITH(cloader::details::LoadSymbolFromLibrary(handle, nullptr), "Symbol name cannot be null or empty");
    CHECK_THROWS_WITH(cloader::details::LoadSymbolFromLibrary(handle, ""), "Symbol name cannot be null or empty");

    dlclose(handle);
}

TEST_CASE("LoadSymbolFromLibrary rejects handles not loaded by cloader")
{
    // A real dlopen handle that LoadLibrary never registered: the symbol is
    // missing, so the loader looks up the handle and finds no filepath.
    void* handle = dlopen(nullptr, RTLD_LAZY | RTLD_LOCAL);
    REQUIRE(handle != nullptr);

    CHECK_THROWS_WITH(cloader::details::LoadSymbolFromLibrary(handle, "cloader_test_missing_symbol_xyz"),
                      "Invalid library handle");

    dlclose(handle);
}

TEST_CASE("UnloadLibrary ignores null handles")
{
    cloader::details::UnloadLibrary(nullptr);
}

#endif
