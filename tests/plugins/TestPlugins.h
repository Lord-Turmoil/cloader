/**
 * @file TestPlugins.h
 * @brief Plugin classes shared by the cloader unit tests.
 */

#pragma once

#include <cloader/cloader.h>

#include <string>

namespace cloader_test
{

// Exports a delete function and reads a display name from args.
class TestPlugin : public cloader::Loadable
{
public:
    explicit TestPlugin(std::string name);
    ~TestPlugin() override;

    const std::string& Name() const;

    static int LiveInstances();
    static int DeleteCalls();

private:
    std::string _name;
};

// Exports no delete function; hosts fall back to the default delete operator.
class PlainPlugin : public cloader::Loadable
{
public:
    PlainPlugin();
    ~PlainPlugin() override;

    static int LiveInstances();
};

// Factory returns nullptr.
class NullPlugin : public cloader::Loadable
{
};

// Factories throw.
class ThrowingPlugin : public cloader::Loadable
{
};

class ThrowingCloaderPlugin : public cloader::Loadable
{
};

} // namespace cloader_test
