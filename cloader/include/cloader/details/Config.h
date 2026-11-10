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

#ifdef CLOADER_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define CLOADER_EXPAND(X)    X
#define CLOADER_STRINGIFY(X) #X

#define CLOADER_NEW_SYMBOL_PREFIX    "Cloader_NewLoadableInstance"
#define CLOADER_DELETE_SYMBOL_PREFIX "Cloader_DeleteLoadableInstance"

#define CLOADER_LOG_LEVEL_ENV_VAR "CLOADER_LOG"
