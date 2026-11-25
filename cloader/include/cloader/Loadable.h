/**
 * @file Loadable.h
 * @brief Base class for all classes that can be loaded by Cloader.
 */

#pragma once

#include "cloader/details/Config.h"
#include "cloader/details/Logging.h"

#include <format>

namespace cloader
{

/**
 * @brief Base class for all classes that is intended to be loaded by Cloader.
 * @details All classes that can be loaded by Cloader must inherit from this class. Each class must also provide a
 * new instance function and an optional delete instance function with C linkage.
 *
 * The new instance function must have the following signature:
 * @code{.cpp}
 * extern "C" cloader::Loadable* Cloader_NewLoadableInstance_<class_name>(void* args);
 * @endcode
 * The \<class_name\> contains the fully qualified class name and namespaces, with "::" replaced by "__". For
 * example, for a class named "MyClass" in the namespace "my::namespace", the new instance function should be named
 * Cloader_NewLoadableInstance_my__namespace__MyClass. The function must return a pointer to a new instance of the
 * class. Returning nullptr will cause the ClassLoader to throw an InstantiationException.
 *
 * The delete instance function is optional, but if provided, it must have the following signature:
 * @code{.cpp}
 * extern "C" void Cloader_DeleteLoadableInstance_<class_name>(cloader::Loadable* instance);
 * @endcode
 * The \<class_name\> is the same as for the new instance function. The function must take care of deleting the instance
 * of the class. If the delete instance function is not provided, the default delete operator will be used to delete the
 * instance.
 *
 * You can use \ref DECLARE_CLOADER_NEW_INSTANCE and \ref DECLARE_CLOADER_DELETE_INSTANCE macros to generate these
 * declarations for you.
 *
 * @see DECLARE_CLOADER_NEW_INSTANCE
 * @see DECLARE_CLOADER_DELETE_INSTANCE
 */
class CLOADER_EXPORT Loadable
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
                        details::logging::LogWarning(
                            std::format("Exception thrown while deleting loadable instance of type {}: {}",
                                        typeid(T).name(), e.what()));
                    }
                    catch (...)
                    {
                        // suppress any exception
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

// Sadly MSVC's macro expansion could not automatically expand __VA_ARGS__...
#define _CLOADER_EXPAND_MACRO(X) X

#define _CLOADER_SELECT_SYMBOL_NAME_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

// clang-format off
#define _CLOADER_GET_SYMBOL_NAME_0(PREFIX)                                                                             \
    PREFIX
#define _CLOADER_GET_SYMBOL_NAME_1(PREFIX, CLASS_NAME)                                                                 \
    PREFIX##_##CLASS_NAME
#define _CLOADER_GET_SYMBOL_NAME_2(PREFIX, N1, CLASS_NAME)                                                             \
    PREFIX##_##N1##__##CLASS_NAME
#define _CLOADER_GET_SYMBOL_NAME_3(PREFIX, N1, N2, CLASS_NAME)                                                         \
    PREFIX##_##N1##__##N2##__##CLASS_NAME
#define _CLOADER_GET_SYMBOL_NAME_4(PREFIX, N1, N2, N3, CLASS_NAME)                                                     \
    PREFIX##_##N1##__##N2##__##N3##__##CLASS_NAME
#define _CLOADER_GET_SYMBOL_NAME_5(PREFIX, N1, N2, N3, N4, CLASS_NAME)                                                 \
    PREFIX##_##N1##__##N2##__##N3##__##N4##__##CLASS_NAME

#define _CLOADER_GET_SYMBOL_NAME(PREFIX, ...) _CLOADER_EXPAND_MACRO(                                                   \
    _CLOADER_SELECT_SYMBOL_NAME_MACRO(                                                                                 \
        __VA_ARGS__,                                                                                                   \
        _CLOADER_GET_SYMBOL_NAME_5,                                                                                    \
        _CLOADER_GET_SYMBOL_NAME_4,                                                                                    \
        _CLOADER_GET_SYMBOL_NAME_3,                                                                                    \
        _CLOADER_GET_SYMBOL_NAME_2,                                                                                    \
        _CLOADER_GET_SYMBOL_NAME_1,                                                                                    \
        _CLOADER_GET_SYMBOL_NAME_0                                                                                     \
    )(PREFIX, __VA_ARGS__)                                                                                             \
)

#define _CLOADER_NEW_INSTANCE_SYMBOL_NAME(...) _CLOADER_EXPAND_MACRO(                                                  \
    _CLOADER_GET_SYMBOL_NAME(Cloader_NewLoadableInstance, __VA_ARGS__)                                                 \
)

#define _CLOADER_DELETE_INSTANCE_SYMBOL_NAME(...) _CLOADER_EXPAND_MACRO(                                               \
    _CLOADER_GET_SYMBOL_NAME(Cloader_DeleteLoadableInstance, __VA_ARGS__)                                              \
)

/**
 * @brief Declares the C-style new instance function for a Loadable class.
 * @details Provide the fully qualified class name and namespaces. For a class named "MyClass" in the namespace
 * "my::namespace", the declaration looks like this:
 * @code{.cpp}
 * DECLARE_CLOADER_NEW_INSTANCE(my, namespace, MyClass);
 * @endcode
 */
#define DECLARE_CLOADER_NEW_INSTANCE(...) _CLOADER_EXPAND_MACRO(                                                       \
    extern "C" CLOADER_EXPORT cloader::Loadable* _CLOADER_NEW_INSTANCE_SYMBOL_NAME(__VA_ARGS__)(void* args)            \
)

/**
 * @brief Declares the C-style delete instance function for a Loadable class.
 * @details Provide the fully qualified class name and namespaces. For a class named "MyClass" in the namespace
 * "my::namespace", the declaration looks like this:
 * @code{.cpp}
 * DECLARE_CLOADER_DELETE_INSTANCE(my, namespace, MyClass);
 * @endcode
 */
#define DECLARE_CLOADER_DELETE_INSTANCE(...) _CLOADER_EXPAND_MACRO(                                                   \
        extern "C" CLOADER_EXPORT void _CLOADER_DELETE_INSTANCE_SYMBOL_NAME(__VA_ARGS__)(cloader::Loadable * instance)\
)

// clang-format on
