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

} // namespace extension
