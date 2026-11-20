# Examples

Cloader ships with an example project that demonstrates the complete workflow of the library: defining a loadable interface, implementing plugins in shared libraries, and loading them at runtime.

## Logger example

The `examples/logger` directory contains a small application that loads logger implementations at runtime and drives them through a common interface.

### The interface

`ILogger.h` defines the plugin interface. It derives from @ref cloader::Loadable, which is required for every class that Cloader can instantiate:

```cpp
class ILogger : public cloader::Loadable
{
public:
    virtual ~ILogger() = 0;

    virtual void OnAppStart();
    virtual void OnAppUpdate();
    virtual void OnAppStop();
};
```

### The plugins

Two plugins implement the interface, each compiled into its own shared library (`libStandardLogger.so` and `libVerboseLogger.so` by `examples/logger/CMakeLists.txt`).

`StandardLogger.cpp` defines `ext::StandardLogger`, which prints `[class] (name) func()` for every callback. It exports a C-style factory using the @ref DECLARE_CLOADER_NEW_INSTANCE macro:

```cpp
DECLARE_CLOADER_NEW_INSTANCE(ext, StandardLogger)
{
    std::string name = args ? static_cast<const char*>(args) : "unknown";
    return new ext::StandardLogger(name);
}
```

The macro expands to the symbol `Cloader_NewLoadableInstance_ext__StandardLogger` that Cloader looks up when instantiating the class. The `args` pointer is forwarded from the application, and is used here as a display name.

`VerboseLogger.cpp` defines `ext::VerboseLogger`, which additionally logs every function entry with `LOG_FUNC()`. It also exports a custom delete function through @ref DECLARE_CLOADER_DELETE_INSTANCE, so Cloader uses it instead of the default `delete` operator when destroying the instance.

### The application

`main.cpp` builds a @ref cloader::ClassLoader for the interface and instantiates a plugin in one of two ways.

From a shared library file and class name, using the constructor `ClassLoader(const char* filepath, const char* className)` followed by @ref cloader::ClassLoader::NewInstance (which forwards the optional `args`
pointer):

```cpp
cloader::ClassLoader<ext::ILogger> loader(filepath, className);
cloader::LoadablePtr<ext::ILogger> logger = loader.NewInstance((void*)args);
```

Or from a factory function pointer, which is used as a built-in fallback (`DummyLogger` in the example):

```cpp
cloader::ClassLoader<ext::ILogger> loader(DummyLogger::NewInstance);
```

The returned @ref cloader::LoadablePtr owns the instance; the interface methods are then invoked like any virtual call:

```cpp
logger->OnAppStart();
logger->OnAppUpdate();
logger->OnAppStop();
```

### Building and running

Build the example with:

```sh
cmake -S . -B build -DCLOADER_BUILD_EXAMPLES=ON
cmake --build build
```

Then run it, loading the `VerboseLogger` plugin from the shared library:

```sh
./scripts/example.sh logger libVerboseLogger.so ext::VerboseLogger "my logger"
```

Or run the built-in `DummyLogger` without a plugin:

```sh
./scripts/example.sh logger "my logger"
```

`scripts/example.sh` puts `build/lib` on the library search path so the loader can find the plugin libraries.
