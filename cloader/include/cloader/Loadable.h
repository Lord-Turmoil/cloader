#pragma once

#include "cloader/details/Logging.h"

#include <format>

namespace cloader
{

/**
 * @brief Base class for all classes that is intended to be loaded by cloader.
 * @details All classes that are intended to be loaded by cloader must inherit from this class. At the same time, each
 * class must provide a new instance function and an optional delete instance function in C style.
 *
 * The new instance function must have the following signature:
 * @code{.cpp}
 * extern "C" cloader::Loadable* Cloader_NewLoadableInstance_<class_name>(void* args);
 * @endcode
 * The <class_name> is contains the fully qualified class name and namespaces, where "::" is replaced with "__". For
 * example, for a class named "MyClass" in the namespace "my::namespace", the new instance function should be named
 * Cloader_NewLoadableInstance_my__namespace__MyClass. The function must return a pointer to a new instance of the
 * class. Returning nullptr will cause the ClassLoader to throw an InstantiationException.
 *
 * The delete instance function is optional, but if provided, it must have the following signature:
 * @code{.cpp}
 * extern "C" void Cloader_DeleteLoadableInstance_<class_name>(cloader::Loadable* instance);
 * @endcode
 * The <class_name> is the same as for the new instance function. The function must take care of deleting the instance
 * of the class. If the delete instance function is not provided, the default delete operator will be used to delete the
 * instance.
 *
 * @see CLOADER_NEW_SYMBOL_PREFIX
 * @see CLOADER_DELETE_SYMBOL_PREFIX
 */
class Loadable
{
public:
    virtual ~Loadable() = default;

public:
    /**
     * @brief Deleter for Loadable objects.
     * @details This deleter is used to delete Loadable objects created by the ClassLoader. It will call the appropriate
     * delete function if provided, otherwise it will use the default delete operator.
     * @tparam T The type of the Loadable object.
     */
    template <typename T>
    requires(std::is_base_of_v<Loadable, T>)
    struct Deleter
    {
        void (*deleter)(Loadable*);

        void operator()(T* loadable) const noexcept
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
