#include "TestPlugins.h"

#include <stdexcept>
#include <utility>

namespace cloader_test
{

namespace
{

int sLiveInstances = 0;
int sDeleteCalls = 0;
int sPlainLiveInstances = 0;

} // namespace

TestPlugin::TestPlugin(std::string name) : _name(std::move(name))
{
    ++sLiveInstances;
}

TestPlugin::~TestPlugin()
{
    --sLiveInstances;
}

const std::string& TestPlugin::Name() const
{
    return _name;
}

int TestPlugin::LiveInstances()
{
    return sLiveInstances;
}

int TestPlugin::DeleteCalls()
{
    return sDeleteCalls;
}

PlainPlugin::PlainPlugin()
{
    ++sPlainLiveInstances;
}

PlainPlugin::~PlainPlugin()
{
    --sPlainLiveInstances;
}

int PlainPlugin::LiveInstances()
{
    return sPlainLiveInstances;
}

} // namespace cloader_test

// The factories live inside the namespace so they can reach the counters in
// the anonymous namespace above; extern "C" keeps the exported symbols
// unmangled either way.
namespace cloader_test
{

DECLARE_CLOADER_NEW_INSTANCE(cloader_test, TestPlugin)
{
    std::string name = args ? static_cast<const char*>(args) : "";
    return new cloader_test::TestPlugin(name);
}

DECLARE_CLOADER_DELETE_INSTANCE(cloader_test, TestPlugin)
{
    delete static_cast<cloader_test::TestPlugin*>(instance);
    ++sDeleteCalls;
}

DECLARE_CLOADER_NEW_INSTANCE(cloader_test, PlainPlugin)
{
    (void)args;
    return new cloader_test::PlainPlugin();
}

DECLARE_CLOADER_NEW_INSTANCE(cloader_test, NullPlugin)
{
    (void)args;
    return nullptr;
}

DECLARE_CLOADER_NEW_INSTANCE(cloader_test, ThrowingPlugin)
{
    (void)args;
    throw std::runtime_error("plugin boom");
}

DECLARE_CLOADER_NEW_INSTANCE(cloader_test, ThrowingCloaderPlugin)
{
    (void)args;
    throw cloader::InvalidArgumentException("factory argument");
}

} // namespace cloader_test
