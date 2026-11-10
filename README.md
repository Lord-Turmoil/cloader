# Cloader

A tiny cross-platform C++ class loader library.

[![CI](https://github.com/Lord-Turmoil/cloader/actions/workflows/ci.yml/badge.svg)](https://github.com/Lord-Turmoil/cloader/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Cloader loads shared libraries at runtime and instantiates C++ classes that
implement a common interface. It is useful for plugin systems: the host
application only knows an abstract interface, while the concrete
implementations live in libraries that can be swapped without rebuilding.

## Features

- Loads shared libraries from a file path at runtime
- Simple API for loading and instantiating classes
- Cross-platform: Linux, macOS and Windows

## Quick start

The plugin defines a class that derives from `cloader::Loadable` and exports a
C-style factory:

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

The host loads and instantiates it like this:

```cpp
cloader::ClassLoader<MyClass> loader("./libMyClass.so", "MyClass");
cloader::LoadablePtr<MyClass> instance = loader.NewInstance(nullptr);
```

See the [logger example](examples/logger) for a complete plugin system with an
interface, two plugins and a host application.

## Building

You need CMake 3.21 or newer and a C++20 compiler.

```sh
cmake -S . -B build -DCLOADER_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build
```

Unit tests live in `tests/` and use [doctest](https://github.com/doctest/doctest),
which CMake fetches automatically on configure. The `cmake --preset debug` and
`cmake --preset release` presets configure builds with examples and tests
enabled.

## Documentation

Generate the API documentation with Doxygen:

```sh
cmake -S . -B build -DCLOADER_BUILD_DOCS=ON
cmake --build build --target docs
```

Open `build/docs/html/index.html` in a browser. Doxygen is required; Graphviz
(`dot`) is optional and enables class and call graphs. The same can be done
with `cmake --preset docs` followed by `cmake --build --preset docs`.

## Coverage

Build with coverage instrumentation and generate an HTML report:

```sh
cmake -S . -B build -DCLOADER_BUILD_COVERAGE=ON
cmake --build build
cmake --build build --target coverage
```

Open `build/coverage/index.html` in a browser. The target runs the unit tests
and turns the collected data into a report with
[gcovr](https://gcovr.com/), which needs to be installed (`pip install gcovr`
works). Coverage is supported with GCC and Clang, not MSVC.

## Using Cloader in your project

Install Cloader and link against it from CMake:

```sh
cmake --install build --prefix /path/to/prefix
```

```cmake
find_package(cloader CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE cloader::cloader)
```

## Contributing

Contributions are welcome. Take a look at [CONTRIBUTING.md](CONTRIBUTING.md)
for build, test and commit conventions.

## License

Cloader is licensed under the MIT License. See [LICENSE](LICENSE) for details.
