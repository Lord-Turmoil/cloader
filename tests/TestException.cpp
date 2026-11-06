#include <cloader/cloader.h>

#include <doctest/doctest.h>

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

TEST_CASE("exceptions derive from std::exception and CloaderException")
{
    static_assert(std::is_base_of_v<std::exception, cloader::CloaderException>);
    static_assert(std::is_base_of_v<cloader::CloaderException, cloader::InvalidArgumentException>);
    static_assert(std::is_base_of_v<cloader::CloaderException, cloader::FileNotFoundException>);
    static_assert(std::is_base_of_v<cloader::CloaderException, cloader::SymbolNotFoundException>);
    static_assert(std::is_base_of_v<cloader::CloaderException, cloader::ClassCastException>);
    static_assert(std::is_base_of_v<cloader::CloaderException, cloader::InstantiationException>);
}

TEST_CASE("CloaderException carries the message")
{
    cloader::CloaderException e("something failed");
    CHECK(e.what() == std::string("something failed"));
    CHECK(e.GetMessage() == "something failed");
}

TEST_CASE("CloaderException accepts a null message")
{
    cloader::CloaderException e(static_cast<const char*>(nullptr));
    CHECK(e.GetMessage().empty());
    CHECK(e.what() == std::string(""));
}

TEST_CASE("FileNotFoundException formats the file path")
{
    cloader::FileNotFoundException e("/no/such/library.so");
    CHECK(e.GetMessage() == "File '/no/such/library.so' not found");
}

TEST_CASE("SymbolNotFoundException formats the class and symbol names")
{
    cloader::SymbolNotFoundException e("foo::Bar", "Cloader_NewLoadableInstance_foo__Bar");
    CHECK(e.GetMessage() == "Class 'foo::Bar' not found (requires symbol: 'Cloader_NewLoadableInstance_foo__Bar')");
}

TEST_CASE("ClassCastException formats the type names")
{
    cloader::ClassCastException e(typeid(int), typeid(double));
    std::string expected = "Cannot cast class '" + std::string(typeid(int).name()) + "' to class '" +
                           std::string(typeid(double).name()) + "'";
    CHECK(e.GetMessage() == expected);
}

TEST_CASE("InstantiationException without a cause")
{
    cloader::InstantiationException e(typeid(int), "factory returned null");
    std::string expected =
        "Failed to instantiate class '" + std::string(typeid(int).name()) + "': factory returned null";
    CHECK(e.GetMessage() == expected);
    CHECK(e.Cause() == nullptr);
}

TEST_CASE("InstantiationException wraps a std exception as the cause")
{
    cloader::InstantiationException e(typeid(int), std::runtime_error("boom"));
    CHECK(e.GetMessage() == "Failed to instantiate class '" + std::string(typeid(int).name()) + "': boom");
    REQUIRE(e.Cause() != nullptr);
    CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), std::exception);
}

TEST_CASE("InstantiationException wraps a CloaderException as the cause")
{
    cloader::InstantiationException e(typeid(int), cloader::InvalidArgumentException("bad input"));
    CHECK(e.GetMessage() == "Failed to instantiate class '" + std::string(typeid(int).name()) + "': bad input");
    REQUIRE(e.Cause() != nullptr);
    CHECK_THROWS_WITH_AS(std::rethrow_exception(e.Cause()), "bad input", cloader::CloaderException);
}

TEST_CASE("InstantiationException from an exception pointer keeps the plain message")
{
    std::exception_ptr cause = std::make_exception_ptr(std::runtime_error("ignored in message"));
    cloader::InstantiationException e(typeid(int), cause);
    CHECK(e.GetMessage() == "Failed to instantiate class '" + std::string(typeid(int).name()) + "'");
    REQUIRE(e.Cause() != nullptr);
    CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), std::runtime_error);
}

TEST_CASE("InvalidArgumentException also accepts a std::string message")
{
    cloader::InvalidArgumentException e(std::string("std::string message"));
    CHECK(e.GetMessage() == "std::string message");
    CHECK(e.what() == std::string("std::string message"));
}

TEST_CASE("InstantiationException accepts a plain message")
{
    cloader::InstantiationException e("plain message");
    CHECK(e.GetMessage() == "plain message");
    CHECK(e.Cause() == nullptr);
}
