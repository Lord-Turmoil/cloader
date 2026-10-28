#include "ILogger.h"

#include <cloader/cloader.h>

#include <iostream>

class DummyLogger : public ext::ILogger
{
public:
    DummyLogger()
    {
        LOG_FUNC();
    }

    ~DummyLogger() override
    {
        LOG_FUNC();
    }

public:
    static Loadable* NewInstance(void* args)
    {
        LOG_FUNC();
        return new DummyLogger();
    }

public:
    void OnAppStart() override
    {
        LOG_FUNC();
    }

    void OnAppUpdate() override
    {
        LOG_FUNC();
    }

    void OnAppStop() override
    {
        LOG_FUNC();
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
    std::unique_ptr<cloader::ClassLoader<ext::ILogger>> loader;
    cloader::LoadablePtr<ext::ILogger> logger;

    const char* args;

    if (argc >= 3)
    {
        const char* filepath = argv[1];
        const char* className = argv[2];
        args = (argc >= 4) ? argv[3] : nullptr;
        try
        {
            loader = std::make_unique<cloader::ClassLoader<ext::ILogger>>(filepath, className);
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
        loader = std::make_unique<cloader::ClassLoader<ext::ILogger>>(DummyLogger::NewInstance);
    }

    try
    {
        logger = loader->NewInstance((void*)args);
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
    logger->OnAppStart();
    std::cout << "===== Updating application =====" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        logger->OnAppUpdate();
    }
    std::cout << "===== Stopping application =====" << std::endl;
    logger->OnAppStop();

    return 0;
}
