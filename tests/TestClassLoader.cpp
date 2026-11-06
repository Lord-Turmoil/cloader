#include <cloader/cloader.h>

#include "plugins/TestPlugins.h"

#include <doctest/doctest.h>

#include <exception>
#include <stdexcept>
#include <string>

namespace
{

class OtherLoadable : public cloader::Loadable
{
};

std::string PluginPath()
{
    return std::string(CLOADER_TEST_PLUGIN_DIR) + "/" + CLOADER_TEST_PLUGIN_NAME;
}

// The plugin library exports failing factories for these classes, so the
// successful paths are exercised here with local factories instead.
cloader::Loadable* MakeNullPlugin(void*)
{
    return new cloader_test::NullPlugin;
}

cloader::Loadable* MakeThrowingPlugin(void*)
{
    return new cloader_test::ThrowingPlugin;
}

cloader::Loadable* MakeThrowingCloaderPlugin(void*)
{
    return new cloader_test::ThrowingCloaderPlugin;
}

int sDirectDeleteCalls = 0;

void CountDirectDelete(cloader::Loadable* instance)
{
    ++sDirectDeleteCalls;
    delete instance;
}

void ThrowingDirectDelete(cloader::Loadable* instance)
{
    delete instance;
    throw std::runtime_error("direct delete failed");
}

} // namespace

TEST_CASE("loads a class from a shared library and forwards args")
{
    cloader::ClassLoader<cloader_test::TestPlugin> loader(PluginPath().c_str(), "cloader_test::TestPlugin");
    int deletesBefore = cloader_test::TestPlugin::DeleteCalls();
    {
        cloader::LoadablePtr<cloader_test::TestPlugin> instance = loader.NewInstance((void*)"unit test");
        REQUIRE(instance != nullptr);
        CHECK(instance->Name() == "unit test");
        CHECK(cloader_test::TestPlugin::LiveInstances() == 1);
    }
    CHECK(cloader_test::TestPlugin::LiveInstances() == 0);
    CHECK(cloader_test::TestPlugin::DeleteCalls() == deletesBefore + 1);
}

TEST_CASE("null args are forwarded as-is")
{
    cloader::ClassLoader<cloader_test::TestPlugin> loader(PluginPath().c_str(), "cloader_test::TestPlugin");
    cloader::LoadablePtr<cloader_test::TestPlugin> instance = loader.NewInstance(nullptr);
    REQUIRE(instance != nullptr);
    CHECK(instance->Name().empty());
}

TEST_CASE("loads a class without a delete function using the default delete")
{
    cloader::ClassLoader<cloader_test::PlainPlugin> loader(PluginPath().c_str(), "cloader_test::PlainPlugin");
    int liveBefore = cloader_test::PlainPlugin::LiveInstances();
    {
        cloader::LoadablePtr<cloader_test::PlainPlugin> instance = loader.NewInstance(nullptr);
        REQUIRE(instance != nullptr);
        CHECK(cloader_test::PlainPlugin::LiveInstances() == liveBefore + 1);
    }
    CHECK(cloader_test::PlainPlugin::LiveInstances() == liveBefore);
}

TEST_CASE("multiple instances from one loader are independent")
{
    cloader::ClassLoader<cloader_test::TestPlugin> loader(PluginPath().c_str(), "cloader_test::TestPlugin");
    int deletesBefore = cloader_test::TestPlugin::DeleteCalls();
    int liveBefore = cloader_test::TestPlugin::LiveInstances();
    {
        cloader::LoadablePtr<cloader_test::TestPlugin> a = loader.NewInstance((void*)"a");
        cloader::LoadablePtr<cloader_test::TestPlugin> b = loader.NewInstance((void*)"b");
        REQUIRE(a != nullptr);
        REQUIRE(b != nullptr);
        CHECK(a->Name() == "a");
        CHECK(b->Name() == "b");
        CHECK(cloader_test::TestPlugin::LiveInstances() == liveBefore + 2);
    }
    CHECK(cloader_test::TestPlugin::LiveInstances() == liveBefore);
    CHECK(cloader_test::TestPlugin::DeleteCalls() == deletesBefore + 2);
}

TEST_CASE("throws FileNotFoundException for a missing library")
{
    CHECK_THROWS_WITH_AS(
        (cloader::ClassLoader<cloader_test::TestPlugin>("/no/such/library.so", "cloader_test::TestPlugin")),
        "File '/no/such/library.so' not found", cloader::FileNotFoundException);
}

TEST_CASE("throws InvalidArgumentException for an empty file path")
{
    CHECK_THROWS_WITH_AS((cloader::ClassLoader<cloader_test::TestPlugin>(nullptr, "cloader_test::TestPlugin")),
                         "Path to the library cannot be null or empty", cloader::InvalidArgumentException);
    CHECK_THROWS_WITH_AS((cloader::ClassLoader<cloader_test::TestPlugin>("", "cloader_test::TestPlugin")),
                         "Path to the library cannot be null or empty", cloader::InvalidArgumentException);
}

TEST_CASE("throws SymbolNotFoundException when the class is not exported")
{
    CHECK_THROWS_WITH_AS(
        (cloader::ClassLoader<cloader_test::TestPlugin>(PluginPath().c_str(), "cloader_test::DoesNotExist")),
        "Class 'cloader_test::DoesNotExist' not found (requires symbol: "
        "'Cloader_NewLoadableInstance_cloader_test__DoesNotExist')",
        cloader::SymbolNotFoundException);
}

TEST_CASE("unqualified class names resolve to unqualified symbols")
{
    CHECK_THROWS_WITH_AS((cloader::ClassLoader<cloader_test::TestPlugin>(PluginPath().c_str(), "TestPlugin")),
                         "Class 'TestPlugin' not found (requires symbol: 'Cloader_NewLoadableInstance_TestPlugin')",
                         cloader::SymbolNotFoundException);
}

TEST_CASE("a factory returning null becomes an InstantiationException")
{
    cloader::ClassLoader<cloader_test::NullPlugin> loader(PluginPath().c_str(), "cloader_test::NullPlugin");
    try
    {
        loader.NewInstance(nullptr);
        FAIL("expected InstantiationException");
    }
    catch (const cloader::InstantiationException& e)
    {
        CHECK(e.GetMessage().find("Instantiated class is null") != std::string::npos);
        CHECK(e.Cause() == nullptr);
    }
}

TEST_CASE("exceptions from a plugin factory are wrapped")
{
    cloader::ClassLoader<cloader_test::ThrowingPlugin> loader(PluginPath().c_str(), "cloader_test::ThrowingPlugin");
    try
    {
        loader.NewInstance(nullptr);
        FAIL("expected InstantiationException");
    }
    catch (const cloader::InstantiationException& e)
    {
        CHECK(e.GetMessage().find("plugin boom") != std::string::npos);
        REQUIRE(e.Cause() != nullptr);
        CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), std::exception);
    }
}

TEST_CASE("CloaderException from a plugin factory is wrapped too")
{
    cloader::ClassLoader<cloader_test::ThrowingCloaderPlugin> loader(PluginPath().c_str(),
                                                                     "cloader_test::ThrowingCloaderPlugin");
    try
    {
        loader.NewInstance(nullptr);
        FAIL("expected InstantiationException");
    }
    catch (const cloader::InstantiationException& e)
    {
        CHECK(e.GetMessage().find("factory argument") != std::string::npos);
        REQUIRE(e.Cause() != nullptr);
        CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), std::exception);
    }
}

TEST_CASE("class cast failures delete the instance through its delete function")
{
    int deletesBefore = cloader_test::TestPlugin::DeleteCalls();
    int liveBefore = cloader_test::TestPlugin::LiveInstances();
    cloader::ClassLoader<OtherLoadable> loader(PluginPath().c_str(), "cloader_test::TestPlugin");
    CHECK_THROWS_AS(loader.NewInstance(nullptr), cloader::ClassCastException);
    CHECK(cloader_test::TestPlugin::DeleteCalls() == deletesBefore + 1);
    CHECK(cloader_test::TestPlugin::LiveInstances() == liveBefore);
}

TEST_CASE("Deleter falls back to delete for plugin classes that never instantiate")
{
    // NullPlugin, ThrowingPlugin and ThrowingCloaderPlugin never produce an
    // instance through NewInstance (null return or factory throw), so their
    // deleters are exercised directly to check the fallback for every type.
    cloader::Loadable::Deleter<cloader_test::NullPlugin>{ nullptr }(new cloader_test::NullPlugin);
    cloader::Loadable::Deleter<cloader_test::ThrowingPlugin>{ nullptr }(new cloader_test::ThrowingPlugin);
    cloader::Loadable::Deleter<cloader_test::ThrowingCloaderPlugin>{ nullptr }(new cloader_test::ThrowingCloaderPlugin);
}

TEST_CASE("NewInstance succeeds for every plugin type given a working factory")
{
    cloader::ClassLoader<cloader_test::NullPlugin> nullLoader(MakeNullPlugin);
    cloader::ClassLoader<cloader_test::ThrowingPlugin> throwingLoader(MakeThrowingPlugin);
    cloader::ClassLoader<cloader_test::ThrowingCloaderPlugin> cloaderThrowingLoader(MakeThrowingCloaderPlugin);

    REQUIRE(nullLoader.NewInstance(nullptr) != nullptr);
    REQUIRE(throwingLoader.NewInstance(nullptr) != nullptr);
    REQUIRE(cloaderThrowingLoader.NewInstance(nullptr) != nullptr);
}

TEST_CASE_TEMPLATE("Deleter covers every configuration for plugin types", T, cloader_test::NullPlugin,
                   cloader_test::ThrowingPlugin, cloader_test::ThrowingCloaderPlugin)
{
    // Default delete, valid and null pointer.
    cloader::Loadable::Deleter<T>{ nullptr }(new T);
    cloader::Loadable::Deleter<T>{ nullptr }(nullptr);

    // Custom delete function: called for valid pointers, skipped for null.
    sDirectDeleteCalls = 0;
    cloader::Loadable::Deleter<T>{ CountDirectDelete }(new T);
    CHECK(sDirectDeleteCalls == 1);
    cloader::Loadable::Deleter<T>{ CountDirectDelete }(nullptr);
    CHECK(sDirectDeleteCalls == 1);

    // Exceptions from the delete function are suppressed. The instance is
    // deleted before throwing so it does not leak.
    cloader::Loadable::Deleter<T>{ ThrowingDirectDelete }(new T);
}
