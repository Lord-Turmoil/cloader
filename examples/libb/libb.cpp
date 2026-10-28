#include "spi.h"

#include <iostream>
#include <string>

namespace libb
{

class BSpi : public extension::Spi
{
public:
    BSpi(std::string message) : _message(std::move(message))
    {
        SPI_LOG();
    }

    ~BSpi() override
    {
        SPI_LOG();
    }

private:
    std::string _message;

public:
    void OnAppStart() override
    {
        std::cout << "[BSpi] OnAppStart" << std::endl;
    }

    void OnAppUpdate() override
    {
        _message.push_back('.');
        std::cout << "[BSpi] OnAppUpdate: " << _message << std::endl;
    }

    void OnAppStop() override
    {
        std::cout << "[BSpi] OnAppStop" << std::endl;
    }
};

} // namespace libb

extern "C" cloader::Loadable* Cloader_NewLoadableInstance_libb__BSpi(void* args)
{
    SPI_LOG();
    std::string msg = args ? static_cast<const char*>(args) : "Err... This is BSpi.";
    return new libb::BSpi(msg);
}

class BadSpi : public cloader::Loadable
{
public:
    BadSpi()
    {
        SPI_LOG();
    }

    ~BadSpi() override
    {
        SPI_LOG();
    }
};

extern "C" cloader::Loadable* Cloader_NewLoadableInstance_BadSpi(void*)
{
    SPI_LOG();
    return new BadSpi();
}

extern "C" void Cloader_DeleteLoadableInstance_BadSpi(cloader::Loadable* instance)
{
    SPI_LOG();
    delete instance;
}
