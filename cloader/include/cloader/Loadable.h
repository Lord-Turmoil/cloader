#pragma once

#include "cloader/details/Logging.h"

#include <format>

namespace cloader
{

class Loadable
{
public:
    virtual ~Loadable() = default;

    virtual const char* LoadableVersion() const;

public:
    template <typename T> struct Deleter
    {
        void (*deleter)(Loadable*);

        void operator()(T* loadable) const
        {
            if (loadable)
            {
                if (deleter)
                {
                    try
                    {
                        deleter(loadable);
                    }
                    catch (const std::exception& e)
                    {
                        // supress any exception
                        details::logging::LogError(
                            std::format("Exception occurred while deleting instance of class '{}': {}",
                                        typeid(T).name(), e.what()));
                    }
                    catch (...)
                    {
                        // supress any exception
                        details::logging::LogError(std::format(
                            "Unknown exception occurred while deleting instance of class '{}'", typeid(T).name()));
                    }
                }
                else
                {
                    delete loadable;
                }
            }
        }
    };
};

} // namespace cloader
