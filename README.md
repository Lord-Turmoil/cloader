# Cloader

> Tiny cross-platform C++ class loader library.

## Documentation

Generate the API documentation with Doxygen:

```sh
cmake -S . -B build -DCLOADER_BUILD_DOCS=ON
cmake --build build --target docs
```

Open `build/docs/html/index.html` in a browser. Doxygen is required;
Graphviz (`dot`) is optional and enables class and call graphs.
