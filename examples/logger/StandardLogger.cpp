#include "ILogger.h"

#include <format>
#include <iostream>
#include <string>

namespace ext
{

class StandardLogger : public ILogger
{
public:
    StandardLogger(std::string name) : _name(std::move(name))
    {
        LOG_FUNC();
    }

    ~StandardLogger() override
    {
        LOG_FUNC();
    }

private:
    std::string _name;

public:
    void OnAppStart() override
    {
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }

    void OnAppUpdate() override
    {
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }

    void OnAppStop() override
    {
        std::cout << std::format("[{}] ({}) {}()", CLS_NAME(), _name, __func__) << std::endl;
    }
};

} // namespace ext

DECLARE_CLOADER_NEW_INSTANCE(ext, StandardLogger)
{
    std::string name = args ? static_cast<const char*>(args) : "unknown";
    return new ext::StandardLogger(name);
}
