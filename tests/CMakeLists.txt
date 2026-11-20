# Unit tests for the cloader library.
#
# doctest is fetched on demand, so building and running the tests needs no
# extra dependency beyond a network connection on the first configure.
include(FetchContent)

# doctest is a test-only dependency: keep it out of cloader's install tree.
set(DOCTEST_NO_INSTALL ON)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.5.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(doctest)

add_library(
    cloader_test_plugins
    SHARED
        plugins/TestPlugins.cpp
        plugins/TestPlugins.h
)
# The test plugins throw cloader's exceptions, so they need the library, not just cloader::headers.
target_link_libraries(cloader_test_plugins PRIVATE cloader::cloader)
cloader_configure_compiler(cloader_test_plugins)

add_executable(
    cloader-tests
    TestException.cpp
    TestLoadable.cpp
    TestLogging.cpp
    TestClassLoader.cpp
    TestNativeLoader.cpp
)
# The native loader tests reach into the internal (not installed) headers.
target_include_directories(
    cloader-tests
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/cloader/src"
)
target_link_libraries(
    cloader-tests
    PRIVATE
        cloader::cloader
        cloader_test_plugins
        doctest::doctest_with_main
)
cloader_configure_compiler(cloader-tests)

# Tell the tests where the plugin library is and what it is called on this
# platform. Asking CMake for the plugin's output directory keeps this correct
# for multi-config generators as well (Visual Studio places artifacts in
# bin/<Config>, while single-config generators place them directly in bin).
target_compile_definitions(
    cloader-tests
    PRIVATE
        CLOADER_TEST_PLUGIN_DIR="$<TARGET_FILE_DIR:cloader_test_plugins>"
        CLOADER_TEST_PLUGIN_NAME="$<TARGET_FILE_NAME:cloader_test_plugins>"
)

# The logging level is read from the environment once per process and cached,
# so each logging scenario runs in its own test process with its own
# environment instead of sharing one.
add_test(
    NAME cloader-tests
    COMMAND cloader-tests --test-case-exclude=log-level-*
)
set_tests_properties(cloader-tests PROPERTIES ENVIRONMENT "CLOADER_LOG=")

foreach(level IN ITEMS disabled invalid debug info warning error)
    add_test(
        NAME cloader-tests-logging-${level}
        COMMAND cloader-tests --test-case=log-level-${level}
    )
endforeach()

# The unset case must run with CLOADER_LOG genuinely absent (not just empty),
# so it unsets the variable instead of setting it.
add_test(
    NAME cloader-tests-logging-unset
    COMMAND "${CMAKE_COMMAND}" -E env --unset=CLOADER_LOG "$<TARGET_FILE:cloader-tests>" --test-case=log-level-unset
)

set_tests_properties(cloader-tests-logging-disabled PROPERTIES ENVIRONMENT "CLOADER_LOG=")
set_tests_properties(cloader-tests-logging-invalid PROPERTIES ENVIRONMENT "CLOADER_LOG=not-a-level")
set_tests_properties(cloader-tests-logging-debug PROPERTIES ENVIRONMENT "CLOADER_LOG=DEBUG")
set_tests_properties(cloader-tests-logging-info PROPERTIES ENVIRONMENT "CLOADER_LOG=i")
set_tests_properties(cloader-tests-logging-warning PROPERTIES ENVIRONMENT "CLOADER_LOG=warn")
set_tests_properties(cloader-tests-logging-error PROPERTIES ENVIRONMENT "CLOADER_LOG=error")

# Every documented alternative spelling selects the same level and reuses the
# same test case; each spelling still needs its own process for the same
# reason as above.
foreach(alias_level IN ITEMS "d debug" "verbose debug" "v debug" "info info" "warning warning" "w warning" "e error")
    string(REPLACE " " ";" alias_parts "${alias_level}")
    list(GET alias_parts 0 alias)
    list(GET alias_parts 1 level)
    add_test(
        NAME cloader-tests-logging-alias-${alias}
        COMMAND cloader-tests --test-case=log-level-${level}
    )
    set_tests_properties(cloader-tests-logging-alias-${alias} PROPERTIES ENVIRONMENT "CLOADER_LOG=${alias}")
endforeach()
