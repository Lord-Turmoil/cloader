#pragma once

#include <cloader/cloader.h>

namespace extension
{

class Spi : public cloader::Loadable
{
public:
    virtual ~Spi() = 0;

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

Spi::~Spi() = default;

#define SPI_LOG()                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        std::cout << __func__ << "()" << std::endl;                                                                    \
    } while (0)

} // namespace extension
