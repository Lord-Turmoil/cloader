#include "spi.h"

#include <iostream>
#include <string>

namespace liba
{

class ASpi : public extension::Spi
{
public:
    ASpi(std::string message) : _message(std::move(message))
    {
        SPI_LOG();
    }

    ~ASpi() override
    {
        SPI_LOG();
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

} // namespace liba

extern "C" cloader::Loadable* Cloader_NewLoadableInstance_liba__ASpi(void* args)
{
    SPI_LOG();

    std::string msg = args ? static_cast<const char*>(args) : "Hello from ASpi!";

    if (msg == "throw")
    {
        throw std::runtime_error("ASpi is naughty");
    }
    else if (msg == "null")
    {
        return nullptr;
    }

    return new liba::ASpi(msg);
}

extern "C" void Cloader_DeleteLoadableInstance_liba__ASpi(cloader::Loadable* instance)
{
    SPI_LOG();

    delete instance;
}
