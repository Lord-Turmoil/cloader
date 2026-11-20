# Cloader

> A tiny cross-platform C++ class loader library.

[![CI](https://github.com/Lord-Turmoil/cloader/actions/workflows/ci.yml/badge.svg)](https://github.com/Lord-Turmoil/cloader/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Cloader offers a simple API to load classes from shared libraries at runtime.

## Quick start

Simply inherit from `cloader::Loadable` and implement a factory function to return a new instance of your class. You may use `args` to pass additional parameters to the factory function, which is provided on calling `NewInstance`.

```cpp
#include <cloader/cloader.h>

class CLOADER_EXPORT MyClass : public cloader::Loadable
{
public:
    // ...
};

DECLARE_CLOADER_NEW_INSTANCE(MyClass) /* void* args */
{
    return new MyClass();
}
```

The host loads and instantiates it like this:

```cpp
cloader::ClassLoader<MyClass> loader("./libMyClass.so", "MyClass"); // "MyClass.dll" on Windows
cloader::LoadablePtr<MyClass> instance = loader.NewInstance(nullptr);
```

See the [logger example](examples/logger) for a complete plugin system with an
interface, two plugins and a host application.

## Building

You need CMake 3.21 or newer and a C++20 compiler.

```sh
cmake -S . -B build -DCLOADER_BUILD_EXAMPLES=ON -DCLOADER_BUILD_TESTS=ON
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

## Using Cloader in your project

Cloader provides two CMake targets:

| Target | Link it from | Contents |
|---|---|---|
| `cloader::cloader` | the application that loads plugins | The static library: `ClassLoader`, the exceptions and the logging |
| `cloader::headers` | plugin libraries | Headers only: `Loadable`, the `DECLARE_CLOADER_*` macros and `CLOADER_EXPORT` |

```cmake
target_link_libraries(my_app PRIVATE cloader::cloader)
target_link_libraries(my_plugin PRIVATE cloader::headers)
```

Plugins that link only `cloader::headers` carry no copy of the loader's code or state. A
plugin that throws Cloader's exceptions or uses `ClassLoader` itself links
`cloader::cloader` instead.

### As a subproject

With FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
    cloader
    GIT_REPOSITORY https://github.com/Lord-Turmoil/cloader.git
    GIT_TAG main # pin a commit for reproducible builds
)
FetchContent_MakeAvailable(cloader)
```

Or, from a git submodule:

```cmake
add_subdirectory(path/to/cloader)
```

As a subproject, Cloader only defines its targets. It leaves the install prefix, the
output directories and the global options alone, and it adds no install rules and no
CTest targets. The tests, examples and documentation are off.

### As an installed package

```sh
cmake --install build --prefix /path/to/prefix
```

```cmake
find_package(cloader 0.1 CONFIG REQUIRED)
```

Point `CMAKE_PREFIX_PATH` at the prefix if it is not a standard location. The targets are
the same as above.

### Options

| Option | Default | Meaning |
|---|---|---|
| `CLOADER_BUILD_TESTS` | `OFF` | Build the unit tests (fetches doctest) |
| `CLOADER_BUILD_EXAMPLES` | `OFF` | Build the example projects |
| `CLOADER_BUILD_DOCS` | `OFF` | Add the `docs` target (needs Doxygen) |
| `CLOADER_BUILD_COVERAGE` | `OFF` | Build with coverage instrumentation; implies the tests |
| `CLOADER_INSTALL` | `ON` at the top level, `OFF` as a subproject | Generate the install rules and the CMake package |

## Contributing

Contributions are welcome. Take a look at [CONTRIBUTING.md](CONTRIBUTING.md)
for build, test and commit conventions.

## License

Cloader is licensed under the MIT License. See [LICENSE](LICENSE) for details.
