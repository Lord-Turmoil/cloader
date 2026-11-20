/**
 * @file Config.h
 * @brief Configuration macros for the Cloader library.
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define CLOADER_PLATFORM_WINDOWS
#elif defined(__linux__)
#define CLOADER_PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define CLOADER_PLATFORM_APPLE
#else
#error "Unsupported platform"
#endif

#if defined(CLOADER_PLATFORM_LINUX) || defined(CLOADER_PLATFORM_APPLE)
#define CLOADER_PLATFORM_UNIX
#endif

/**
 * @brief Marks a symbol for export from a plugin library.
 * @details Plugin factories must be visible to the host's runtime symbol lookup. On Windows nothing is exported from a
 * DLL unless it is explicitly marked; on Unix-like systems the default visibility already exports everything, but the
 * attribute is kept so plugins keep working under -fvisibility=hidden.
 */
#if defined(CLOADER_PLATFORM_WINDOWS)
#define CLOADER_EXPORT __declspec(dllexport)
#else
#define CLOADER_EXPORT __attribute__((visibility("default")))
#endif

#define CLOADER_EXPAND(X)    X
#define CLOADER_STRINGIFY(X) #X

#define CLOADER_NEW_SYMBOL_PREFIX    "Cloader_NewLoadableInstance"
#define CLOADER_DELETE_SYMBOL_PREFIX "Cloader_DeleteLoadableInstance"

#define CLOADER_LOG_LEVEL_ENV_VAR "CLOADER_LOG"
