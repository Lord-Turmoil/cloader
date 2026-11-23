/**
 * @file ClassLoader.h
 * @brief Class loader for the Cloader library.
 */

#pragma once

#include "cloader/Loadable.h"

#include "cloader/Exception.h"
#include "cloader/details/Logging.h"

#include <exception>
#include <format>
#include <memory>
#include <string>

namespace cloader
{

using NewInstanceFn = cloader::Loadable* (*)(void*);
using DeleteInstanceFn = void (*)(cloader::Loadable*);

/**
 * @brief Common base of all class loaders.
 * @note This base wraps the low-level operations of loading a class from a dynamic library.
 */
class ClassLoaderBase
{
public:
    /**
     * @brief Construct a new ClassLoaderBase object.
     * @param filepath The path to the dynamic library file.
     * @param className The name of the class to load from the dynamic library.
     * @throws InvalidArgumentException If the filepath or className is null or empty.
     * @throws FileNotFoundException If the dynamic library file is not found.
     * @throws SymbolNotFoundException If the new instance symbol is not found in the dynamic library.
     */
    ClassLoaderBase(const char* filepath, const char* className);

    /**
     * @brief Construct a new ClassLoaderBase given the specified new instance and delete instance functions.
     * @param newInstanceFn The function pointer to the new instance function.
     * @param deleteInstanceFn The function pointer to the delete instance function.
     * @throws InvalidArgumentException If the newInstanceFn is null.
     * @note deleteInstanceFn can be null, in which case the default delete operator will be used to delete the
     * instance.
     */
    ClassLoaderBase(NewInstanceFn newInstanceFn, DeleteInstanceFn deleteInstanceFn);

    /**
     * @brief Construct a new ClassLoaderBase given the specified new instance function.
     * @param newInstanceFn The function pointer to the new instance function.
     * @throws InvalidArgumentException If the newInstanceFn is null.
     * @note The delete instance function will be set to null, in which case the default delete operator will be used to
     * delete the instance.
     */
    ClassLoaderBase(NewInstanceFn newInstanceFn);

    ClassLoaderBase(const ClassLoaderBase&) = delete;
    ClassLoaderBase& operator=(const ClassLoaderBase&) = delete;
    ClassLoaderBase(ClassLoaderBase&&) = delete;
    ClassLoaderBase& operator=(ClassLoaderBase&&) = delete;

    /**
     * @brief Destroy the ClassLoaderBase object.
     * @warning This will also unload the dynamic library. Make sure it lives longer than any instances of the class
     * loaded from the library.
     */
    ~ClassLoaderBase();

private:
    void* _handle;

protected:
    NewInstanceFn _newInstanceFn;
    DeleteInstanceFn _deleteInstanceFn;
};

/**
 * @brief Strongly-typed class loader.
 * @details It loads the dynamic library on construction and instantiates classes from it.
 * @tparam T The type of the class to load. This must be a subclass of Loadable.
 * @tparam D The deleter type for the class.
 * @warning A ClassLoader owns the loaded dynamic library, so it must outlive every instance created from it.
 */
template <typename T, typename D = Loadable::Deleter<T>>
requires(std::is_base_of_v<Loadable, T>)
class ClassLoader : public ClassLoaderBase
{
public:
    using InstancePtr = std::unique_ptr<T, D>;

    /**
     * @brief Loads the class from the given dynamic library.
     * @details Loads the class from the given dynamic library. Whether the class actually matches T is only checked
     * when NewInstance is called.
     * @param filepath The path to the dynamic library file. The file is looked up in the system library search path.
     * @param className The name of the class to load from the dynamic library.
     * @throws InvalidArgumentException If the filepath or className is null or empty.
     * @throws FileNotFoundException If the dynamic library file is not found.
     * @throws SymbolNotFoundException If the new instance symbol is not found in the dynamic library.
     * @see cloader::Loadable
     */
    ClassLoader(const char* filepath, const char* className);

    /**
     * @brief Construct a new ClassLoader given the specified new instance and delete instance functions.
     * @details This constructor is mainly used to wrap factory functions of classes that are not loaded from a
     * dynamic library.
     * @param newInstanceFn The function pointer to the new instance function.
     * @param deleteInstanceFn The function pointer to the delete instance function.
     * @throws InvalidArgumentException If the newInstanceFn is null.
     * @note deleteInstanceFn can be null, in which case the default delete operator will be used to delete the
     * instance.
     * @see cloader::Loadable
     */
    ClassLoader(NewInstanceFn newInstanceFn, DeleteInstanceFn deleteInstanceFn);

    /**
     * @brief Construct a new ClassLoader given the specified new instance function.
     * @details See the other constructor for details.
     * @param newInstanceFn The function pointer to the new instance function.
     * @throws InvalidArgumentException If the newInstanceFn is null.
     * @note The delete instance function will be set to null, in which case the default delete operator will be used to
     * delete the instance.
     * @see cloader::Loadable
     */
    ClassLoader(NewInstanceFn newInstanceFn);

    /**
     * @brief Create a new instance of the class.
     * @details Calls the new instance function and checks that the result is of type T. Any exception thrown during
     * instantiation is wrapped in an InstantiationException; see \ref InstantiationException::Cause.
     * @param args The arguments to pass to the new instance function.
     * @return A unique pointer to the new instance.
     * @throws InstantiationException If the new instance cannot be created.
     * @see cloader::Loadable
     * @note The arguments are forwarded to the new instance function as-is. Making sure they are correct is the
     * caller's responsibility; the library does not validate them, though invalid arguments may lead to exceptions.
     */
    InstancePtr NewInstance(void* args);
};

/**
 * @brief An alias for a unique pointer to a Loadable object.
 * @tparam T The type of the Loadable object. This must be a subclass of Loadable.
 * @tparam D The deleter type for the Loadable object.
 * @note This alias is provided for convenience, and is equivalent to \ref ClassLoader<T, D>::InstancePtr.
 * @see cloader::Loadable
 * @see cloader::ClassLoader
 * @see cloader::ClassLoader<T, D>::InstancePtr
 */
template <typename T, typename D = Loadable::Deleter<T>>
requires(std::is_base_of_v<Loadable, T>)
using LoadablePtr = ClassLoader<T, D>::InstancePtr;

template <typename T, typename D>
requires(std::is_base_of_v<Loadable, T>)
ClassLoader<T, D>::ClassLoader(const char* filepath, const char* className) : ClassLoaderBase(filepath, className)
{
    details::logging::LogInfo(std::format("Loaded class '{}' in '{}'", className, filepath));
}

template <typename T, typename D>
requires(std::is_base_of_v<Loadable, T>)
ClassLoader<T, D>::ClassLoader(NewInstanceFn newInstanceFn, DeleteInstanceFn deleteInstanceFn)
    : ClassLoaderBase(newInstanceFn, deleteInstanceFn)
{
    details::logging::LogInfo(std::format("Loaded internal class '{}'", typeid(T).name()));
}

template <typename T, typename D>
requires(std::is_base_of_v<Loadable, T>)
ClassLoader<T, D>::ClassLoader(NewInstanceFn factory) : ClassLoaderBase(factory)
{
    details::logging::LogInfo(std::format("Loaded internal class '{}'", typeid(T).name()));
}

template <typename T, typename D>
requires(std::is_base_of_v<Loadable, T>)
typename ClassLoader<T, D>::InstancePtr ClassLoader<T, D>::NewInstance(void* args)
{
    Loadable* loadable;

    try
    {
        details::logging::LogInfo(std::format("Instantiating class '{}'", typeid(T).name()));
        loadable = _newInstanceFn(args);
    }
    catch (const InstantiationException& e)
    {
        throw InstantiationException(typeid(T), e);
    }
    catch (const std::exception& e)
    {
        throw InstantiationException(typeid(T), e);
    }
    catch (...)
    {
        throw InstantiationException(typeid(T), std::current_exception());
    }

    if (!loadable)
    {
        throw InstantiationException(typeid(T), "Instantiated class is null");
    }

    T* instance = dynamic_cast<T*>(loadable);
    if (!instance)
    {
        Loadable::Deleter<Loadable>{ _deleteInstanceFn }(loadable);
        throw ClassCastException(typeid(Loadable), typeid(T));
    }

    return InstancePtr(instance, { _deleteInstanceFn });
}

} // namespace cloader
