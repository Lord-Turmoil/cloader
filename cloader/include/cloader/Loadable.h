#pragma once

namespace cloader
{

class Loadable
{
public:
    virtual ~Loadable() = default;

    virtual const char* LoadableVersion() const;
};

} // namespace cloader
