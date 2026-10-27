#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define CLOADER_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__APPLE__)
#define CLOADER_PLATFORM_UNIX
#else
#error "Unsupported platform"
#endif

#define CLOADER_SYMBOL_PREFIX     "Cloader_NewLoadableInstance"
#define CLOADER_LOG_LEVEL_ENV_VAR "CLOADER_LOG"
