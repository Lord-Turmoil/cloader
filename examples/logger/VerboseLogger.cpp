#include "ILogger.h"

#include <format>
#include <iostream>
#include <string>

namespace ext
{

class VerboseLogger : public ILogger
{
public:
    VerboseLogger(std::string name) : _name(std::move(name))
    {
        LOG_FUNC();
    }

    ~VerboseLogger() override
    {
        LOG_FUNC();
    }

private:
    std::string _name;

public:
    void OnAppStart() override
    {
        LOG_FUNC();
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }

    void OnAppUpdate() override
    {
        LOG_FUNC();
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }

    void OnAppStop() override
    {
        LOG_FUNC();
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }
};

} // namespace ext

DECLARE_CLOADER_NEW_INSTANCE(ext, VerboseLogger)
{
    LOG_FUNC();
    std::string name = args ? static_cast<const char*>(args) : "unknown";
    return new ext::VerboseLogger(name);
}

DECLARE_CLOADER_DELETE_INSTANCE(ext, VerboseLogger)
{
    LOG_FUNC();
    delete instance;
}
