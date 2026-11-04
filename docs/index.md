# Cloader

> Tiny cross-platform C++ class loader library.

Cloader loads shared libraries at runtime and instantiates classes that
implement the @ref cloader::Loadable interface.

## Features

- Load shared libraries from a file path at runtime
- Instantiate classes through the @ref cloader::Loadable interface
- RAII cleanup of libraries and loaded instances
- Typed exceptions for every failure mode

## Quick start

Implement the @ref cloader::Loadable interface and export a C-style factory:

```cpp
#include <cloader/cloader.h>

class MyClass : public cloader::Loadable
{
public:
    // ...
};

DECLARE_CLOADER_NEW_INSTANCE(MyClass)
{
    return new MyClass();
}
```

Load and instantiate it from another binary:

```cpp
cloader::ClassLoader<MyClass> loader("./libMyClass.so", "MyClass");
cloader::LoadablePtr<MyClass> instance = loader.NewInstance(nullptr);
```

See @ref md_docs_2examples for a walkthrough of the logger example that ships
with the repository.

## Building

```sh
cmake -S . -B build -DCLOADER_BUILD_EXAMPLES=ON
cmake --build build
```

## API overview

- @ref cloader::ClassLoader — strongly-typed class loader
- @ref cloader::Loadable — interface implemented by loadable classes
- @ref cloader::CloaderException — base class for all cloader exceptions
