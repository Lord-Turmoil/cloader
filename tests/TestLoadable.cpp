#include <cloader/cloader.h>

#include <doctest/doctest.h>

#include <stdexcept>
#include <string>
#include <type_traits>

namespace
{

class LocalPlugin : public cloader::Loadable
{
public:
    explicit LocalPlugin(int* arg)
    {
        if (arg)
        {
            _value = *arg;
        }
        ++sLiveInstances;
    }

    ~LocalPlugin() override
    {
        --sLiveInstances;
    }

    int Value() const
    {
        return _value;
    }

    static int sLiveInstances;

private:
    int _value = 0;
};

int LocalPlugin::sLiveInstances = 0;

class OtherLoadable : public cloader::Loadable
{
};

int sLocalDeleteCalls = 0;

cloader::Loadable* MakeLocal(void* args)
{
    return new LocalPlugin(static_cast<int*>(args));
}

void DeleteLocal(cloader::Loadable* instance)
{
    ++sLocalDeleteCalls;
    delete instance;
}

void ThrowingDelete(cloader::Loadable* instance)
{
    delete instance;
    throw std::runtime_error("delete failed");
}

cloader::Loadable* ThrowStdException(void*)
{
    throw std::runtime_error("factory boom");
}

cloader::Loadable* ThrowInt(void*)
{
    throw 42;
}

} // namespace

TEST_CASE("Deleter falls back to delete when no function is provided")
{
    cloader::Loadable::Deleter<LocalPlugin> deleter{ nullptr };
    CHECK(LocalPlugin::sLiveInstances == 0);
    deleter(new LocalPlugin(nullptr));
    CHECK(LocalPlugin::sLiveInstances == 0);
}

TEST_CASE("Deleter calls the provided delete function")
{
    sLocalDeleteCalls = 0;
    cloader::Loadable::Deleter<LocalPlugin> deleter{ DeleteLocal };
    deleter(new LocalPlugin(nullptr));
    CHECK(sLocalDeleteCalls == 1);
    CHECK(LocalPlugin::sLiveInstances == 0);
}

TEST_CASE("Deleter ignores null pointers")
{
    sLocalDeleteCalls = 0;
    cloader::Loadable::Deleter<LocalPlugin> deleter{ DeleteLocal };
    deleter(nullptr);
    CHECK(sLocalDeleteCalls == 0);
}

TEST_CASE("Deleter suppresses exceptions from the delete function")
{
    cloader::Loadable::Deleter<LocalPlugin> deleter{ ThrowingDelete };
    deleter(new LocalPlugin(nullptr)); // must not throw
}

TEST_CASE("ClassLoader cannot be copied or moved")
{
    using Loader = cloader::ClassLoader<LocalPlugin>;
    static_assert(!std::is_copy_constructible_v<Loader>);
    static_assert(!std::is_copy_assignable_v<Loader>);
    static_assert(!std::is_move_constructible_v<Loader>);
    static_assert(!std::is_move_assignable_v<Loader>);
}

TEST_CASE("ClassLoader instantiates from a factory function and forwards args")
{
    sLocalDeleteCalls = 0;
    cloader::ClassLoader<LocalPlugin> loader(MakeLocal, DeleteLocal);

    int value = 7;
    cloader::LoadablePtr<LocalPlugin> instance = loader.NewInstance(&value);
    REQUIRE(instance != nullptr);
    CHECK(instance->Value() == 7);
    CHECK(LocalPlugin::sLiveInstances == 1);

    instance.reset();
    CHECK(sLocalDeleteCalls == 1);
    CHECK(LocalPlugin::sLiveInstances == 0);
}

TEST_CASE("ClassLoader with a default delete function deletes through delete")
{
    cloader::ClassLoader<LocalPlugin> loader(MakeLocal);
    {
        cloader::LoadablePtr<LocalPlugin> instance = loader.NewInstance(nullptr);
        REQUIRE(instance != nullptr);
        CHECK(LocalPlugin::sLiveInstances == 1);
    }
    CHECK(LocalPlugin::sLiveInstances == 0);
}

TEST_CASE("ClassLoader factory constructors reject a null function")
{
    using NewInstanceFn = cloader::ClassLoaderBase::NewInstanceFn;
    CHECK_THROWS_AS((cloader::ClassLoader<LocalPlugin>(static_cast<NewInstanceFn>(nullptr))),
                    cloader::InvalidArgumentException);
    CHECK_THROWS_AS((cloader::ClassLoader<LocalPlugin>(nullptr, DeleteLocal)), cloader::InvalidArgumentException);
}

TEST_CASE("NewInstance wraps std exceptions thrown by the factory")
{
    cloader::ClassLoader<LocalPlugin> loader(ThrowStdException);
    try
    {
        loader.NewInstance(nullptr);
        FAIL("expected InstantiationException");
    }
    catch (const cloader::InstantiationException& e)
    {
        CHECK(e.GetMessage().find("factory boom") != std::string::npos);
        REQUIRE(e.Cause() != nullptr);
        CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), std::exception);
    }
}

TEST_CASE("NewInstance wraps non-std exceptions thrown by the factory")
{
    cloader::ClassLoader<LocalPlugin> loader(ThrowInt);
    try
    {
        loader.NewInstance(nullptr);
        FAIL("expected InstantiationException");
    }
    catch (const cloader::InstantiationException& e)
    {
        REQUIRE(e.Cause() != nullptr);
        CHECK_THROWS_AS(std::rethrow_exception(e.Cause()), int);
    }
}

TEST_CASE("NewInstance fails when the factory returns null")
{
    cloader::ClassLoader<LocalPlugin> loader([](void*) -> cloader::Loadable* { return nullptr; });
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

TEST_CASE("NewInstance throws ClassCastException when the instance does not derive from T")
{
    int liveBefore = LocalPlugin::sLiveInstances;
    cloader::ClassLoader<OtherLoadable> loader(MakeLocal);
    CHECK_THROWS_AS(loader.NewInstance(nullptr), cloader::ClassCastException);
    CHECK(LocalPlugin::sLiveInstances == liveBefore);
}
