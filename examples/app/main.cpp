#include "spi.h"

#include <cloader/cloader.h>

#include <iostream>

class DefaultSpi : public extension::Spi
{
public:
    DefaultSpi() = default;
    ~DefaultSpi() override = default;

    static Loadable* NewInstance(void*)
    {
        return new DefaultSpi();
    }

public:
    void OnAppStart() override
    {
        std::cout << "[Default] App started." << std::endl;
    }

    void OnAppUpdate() override
    {
        std::cout << "[Default] App updating..." << std::endl;
    }

    void OnAppStop() override
    {
        std::cout << "[Default] App stopped." << std::endl;
    }
};

void usage()
{
    std::cout << "Usage: app <filepath> <class name> [args]" << std::endl;
    std::cout << "       app [args]" << std::endl;

    exit(1);
}

int main(int argc, char* argv[])
{
    std::unique_ptr<cloader::ClassLoader<extension::Spi>> loader;
    cloader::LoadablePtr<extension::Spi> spi;
    const char* args;

    if (argc >= 3)
    {
        const char* filepath = argv[1];
        const char* className = argv[2];
        args = (argc >= 4) ? argv[3] : nullptr;
        try
        {
            loader = std::make_unique<cloader::ClassLoader<extension::Spi>>(filepath, className);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        catch (...)
        {
            std::cerr << "Error: Unknown exception occurred." << std::endl;
            return 1;
        }
    }
    else
    {
        args = (argc >= 2) ? argv[1] : nullptr;
        loader = std::make_unique<cloader::ClassLoader<extension::Spi>>(DefaultSpi::NewInstance);
    }

    try
    {
        spi = loader->NewInstance((void*)args);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Error: Unknown exception occurred." << std::endl;
        return 1;
    }

    std::cout << "===== Starting application =====" << std::endl;
    spi->OnAppStart();
    std::cout << "===== Updating application =====" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        spi->OnAppUpdate();
    }
    std::cout << "===== Stopping application =====" << std::endl;
    spi->OnAppStop();

    return 0;
}
