#include "spi.h"

#include <iostream>
#include <string>

namespace exta
{

class ASpi : public extension::Spi
{
public:
    ASpi(std::string message) : _message(std::move(message))
    {
        std::cout << "Aspi(" << _message << ")" << std::endl;
    }

    ~ASpi() override
    {
        std::cout << "~ASpi()" << std::endl;
    }

    static Loadable* NewInstance(void* args);

private:
    std::string _message;

public:
    void OnAppStart() override
    {
        std::cout << "[ASpi] OnAppStart: " << _message << std::endl;
    }

    void OnAppUpdate() override
    {
        std::cout << "[ASpi] OnAppUpdate: " << _message << std::endl;
    }

    void OnAppStop() override
    {
        std::cout << "[ASpi] OnAppStop: " << _message << std::endl;
    }
};

} // namespace exta

extern "C" cloader::Loadable* Cloader_NewLoadableInstance_exta__ASpi(void* args)
{
    std::string msg = args ? static_cast<const char*>(args) : "Hello from ASpi!";

    if (msg == "throw")
    {
        throw std::runtime_error("ASpi: throw");
    }
    else if (msg == "null")
    {
        return nullptr;
    }

    return new exta::ASpi(msg);
}

extern "C" void Cloader_DeleteLoadableInstance_exta__ASpi(cloader::Loadable* instance)
{
    std::cout << "Custom delete function called for ASpi instance." << std::endl;

    delete instance;
}
