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

class ClassLoaderBase
{
public:
    using NewInstanceFn = cloader::Loadable* (*)(void*);
    using DeleteInstanceFn = void (*)(cloader::Loadable*);

    ClassLoaderBase(const char* filepath, const char* className);
    ClassLoaderBase(NewInstanceFn newInstanceFn, DeleteInstanceFn deleteInstanceFn);
    ClassLoaderBase(NewInstanceFn newInstanceFn);

    ClassLoaderBase(const ClassLoaderBase&) = delete;
    ClassLoaderBase& operator=(const ClassLoaderBase&) = delete;
    ClassLoaderBase(ClassLoaderBase&&) = delete;
    ClassLoaderBase& operator=(ClassLoaderBase&&) = delete;

    ~ClassLoaderBase();

private:
    void* _handle;

protected:
    NewInstanceFn _newInstanceFn;
    DeleteInstanceFn _deleteInstanceFn;
};

template <typename T, typename D = Loadable::Deleter<T>> class ClassLoader : public ClassLoaderBase
{
public:
    using InstancePtr = std::unique_ptr<T, D>;

    ClassLoader(const char* filepath, const char* className);
    ClassLoader(NewInstanceFn factory);

    InstancePtr NewInstance(void* args);
};

template <typename T, typename D>
ClassLoader<T, D>::ClassLoader(const char* filepath, const char* className) : ClassLoaderBase(filepath, className)
{
    details::logging::LogInfo(std::format("Loaded class '{}' in '{}'", className, filepath));
}

template <typename T, typename D> ClassLoader<T, D>::ClassLoader(NewInstanceFn factory) : ClassLoaderBase(factory)
{
    details::logging::LogInfo(std::format("Loaded internal class '{}'", typeid(T).name()));
}

template <typename T, typename D> typename ClassLoader<T, D>::InstancePtr ClassLoader<T, D>::NewInstance(void* args)
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
        delete loadable;
        throw ClassCastException(typeid(Loadable), typeid(T));
    }

    return InstancePtr(instance, D{ _deleteInstanceFn });
}

} // namespace cloader
