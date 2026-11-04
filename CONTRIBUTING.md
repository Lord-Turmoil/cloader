# Contributing to Cloader

Thanks for taking an interest in the project. Contributions in any form - bug
reports, documentation fixes, new features - are welcome.

## Getting started

You need:

- CMake 3.21 or newer
- a C++ 20 compiler (GCC, Clang or MSVC)
- Doxygen and Graphviz if you want to build the documentation

Fork the repository, clone your fork, and build with the presets:

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset default
```

## Making changes

- `cloader/` holds the library itself, `examples/` the example projects and
  `docs/` the Doxygen configuration and pages.
- Source lists in `cloader/CMakeLists.txt` are explicit. If you add a new
  source file, add it to the list in the same commit - the build will not pick
  it up otherwise.
- Run the formatter before committing:

  ```sh
  ./scripts/format.sh
  ```

  This formats `cloader/` and `examples/` with clang-format using the
  configuration in `.clang-format`.

- If you changed public headers, rebuild the documentation and make sure
  Doxygen reports no warnings:

  ```sh
  cmake --preset docs
  cmake --build --preset docs
  ```

## Committing

- Keep commits small and focused; one logical change per commit.
- Use an emoji prefix in the subject line, matching the existing history
  (`✨`, `🔧`, `📄`, `🐛` and so on).
- Update `CHANGELOG.md` in the same commit when the change is user-visible.

## Pull requests

Open a pull request against `main`. CI builds and tests the library on Linux,
macOS and Windows, and builds the documentation - make sure it passes before
asking for a review.
