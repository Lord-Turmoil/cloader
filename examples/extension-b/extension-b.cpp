#include "spi.h"

#include <iostream>
#include <string>

namespace extb
{

class BSpi : public extension::Spi
{
public:
    BSpi(std::string message) : _message(std::move(message))
    {
        std::cout << "BSpi(" << _message << ")" << std::endl;
    }

    ~BSpi() override
    {
        std::cout << "~BSpi()" << std::endl;
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

} // namespace extb

extern "C" cloader::Loadable* Cloader_NewLoadableInstance_extb__BSpi(void* args)
{
    const char* msg = args ? static_cast<const char*>(args) : "Err... This is BSpi.";
    return new extb::BSpi(std::string(msg));
}
