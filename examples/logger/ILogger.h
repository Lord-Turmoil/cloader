#pragma once

#include <cloader/cloader.h>

namespace ext
{

class ILogger : public cloader::Loadable
{
public:
    virtual ~ILogger() = 0;

public:
    virtual void OnAppStart()
    {
    }

    virtual void OnAppUpdate()
    {
    }

    virtual void OnAppStop()
    {
    }
};

ILogger::~ILogger() = default;

} // namespace ext

#define LOG_FUNC()                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        std::cout << __func__ << "()" << std::endl;                                                                    \
    } while (0)

#define CLS_NAME() typeid(*this).name()
