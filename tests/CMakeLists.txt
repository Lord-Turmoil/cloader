# Unit tests for the cloader library.
#
# doctest is fetched on demand, so building and running the tests needs no
# extra dependency beyond a network connection on the first configure.
include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.5.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(doctest)

# A shared library exporting the plugin classes the loader tests load at
# runtime, mirroring how a real host application uses the library.
add_library(
    cloader_test_plugins
    SHARED
        plugins/TestPlugins.cpp
        plugins/TestPlugins.h
)
target_link_libraries(cloader_test_plugins PRIVATE cloader::cloader)
cloader_configure_compiler(cloader_test_plugins)

add_executable(
    cloader-tests
    TestException.cpp
    TestLoadable.cpp
    TestLogging.cpp
    TestClassLoader.cpp
)
target_include_directories(cloader-tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(
    cloader-tests
    PRIVATE
        cloader::cloader
        cloader_test_plugins
        doctest::doctest_with_main
)
cloader_configure_compiler(cloader-tests)

# Tell the tests where the plugin library is and what it is called on this
# platform. On Windows shared libraries land in the runtime directory.
if(WIN32)
    set(cloader_test_plugin_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
    set(cloader_test_plugin_dir "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif()
target_compile_definitions(
    cloader-tests
    PRIVATE
        CLOADER_TEST_PLUGIN_DIR="${cloader_test_plugin_dir}"
        CLOADER_TEST_PLUGIN_NAME="${CMAKE_SHARED_LIBRARY_PREFIX}cloader_test_plugins${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

# The logging level is read from the environment once per process and cached,
# so each logging scenario runs in its own test process with its own
# environment instead of sharing one.
add_test(
    NAME cloader-tests
    COMMAND cloader-tests --test-case-exclude=log-level-*
)
set_tests_properties(cloader-tests PROPERTIES ENVIRONMENT "CLOADER_LOG=")

foreach(level IN ITEMS disabled invalid debug info error)
    add_test(
        NAME cloader-tests-logging-${level}
        COMMAND cloader-tests --test-case=log-level-${level}
    )
endforeach()

set_tests_properties(cloader-tests-logging-disabled PROPERTIES ENVIRONMENT "CLOADER_LOG=")
set_tests_properties(cloader-tests-logging-invalid PROPERTIES ENVIRONMENT "CLOADER_LOG=not-a-level")
set_tests_properties(cloader-tests-logging-debug PROPERTIES ENVIRONMENT "CLOADER_LOG=DEBUG")
set_tests_properties(cloader-tests-logging-info PROPERTIES ENVIRONMENT "CLOADER_LOG=i")
set_tests_properties(cloader-tests-logging-error PROPERTIES ENVIRONMENT "CLOADER_LOG=error")
