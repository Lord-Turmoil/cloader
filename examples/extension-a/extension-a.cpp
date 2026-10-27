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
    const char* msg = args ? static_cast<const char*>(args) : "Hello from ASpi!";
    return new exta::ASpi(std::string(msg));
}
