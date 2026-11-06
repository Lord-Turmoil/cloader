function(cloader_enable_warnings target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot enable warnings: target '${target}' does not exist")
    endif()

    if(MSVC)
        target_compile_options(
            "${target}"
            PRIVATE
                /W4
                /permissive-
        )
    else()
        target_compile_options(
            "${target}"
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()

function(cloader_configure_compiler target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot apply compiler settings: target '${target}' does not exist")
    endif()

    target_compile_features("${target}" PUBLIC cxx_std_20)

    if(PROJECT_IS_TOP_LEVEL)
        cloader_enable_warnings("${target}")
    endif()

    # Coverage data is gathered by running the instrumented unit tests; see
    # cloader_configure_coverage below for the "coverage" target that turns it
    # into a report. The link flag is PUBLIC so consumers of the static
    # library get the coverage runtime at link time.
    if(CLOADER_BUILD_COVERAGE AND NOT MSVC)
        target_compile_options("${target}" PRIVATE --coverage)
        target_link_options("${target}" PUBLIC --coverage)
    endif()
endfunction()

# Code coverage reporting.
#
# The "coverage" target runs the unit tests under GCC/Clang coverage
# instrumentation and turns the collected data into an HTML report. The report
# is written to ${CMAKE_BINARY_DIR}/coverage (override with
# CLOUDER_COVERAGE_OUTPUT_DIR).
#
# Requires gcovr (https://gcovr.com/), available as a system package or via
# "pip install gcovr".
function(cloader_configure_coverage)
    if(NOT CLOADER_BUILD_COVERAGE)
        return()
    endif()

    if(MSVC)
        message(FATAL_ERROR "CLOADER_BUILD_COVERAGE is not supported with MSVC; use GCC or Clang")
    endif()

    if(NOT BUILD_TESTING)
        message(WARNING "CLOADER_BUILD_COVERAGE is ON but BUILD_TESTING is OFF; the report will be empty")
    endif()

    find_program(CLOADER_COVERAGE_GCOVR_EXECUTABLE gcovr)
    if(NOT CLOADER_COVERAGE_GCOVR_EXECUTABLE)
        message(FATAL_ERROR "CLOADER_BUILD_COVERAGE is ON but gcovr was not found. "
                            "Install it with 'pip install gcovr'")
    endif()

    set(
        CLOADER_COVERAGE_OUTPUT_DIR
        "${CMAKE_BINARY_DIR}/coverage"
        CACHE PATH "Directory the coverage report is written to"
    )

    # Only the library itself is interesting; tests, examples and fetched
    # dependencies are filtered out of the report.
    set(cloader_coverage_filter "${PROJECT_SOURCE_DIR}/cloader/")

    message(STATUS "Coverage reporting enabled - report in ${CLOADER_COVERAGE_OUTPUT_DIR}")

    add_custom_target(
        coverage
        # Drop stale coverage data so the report only reflects this run.
        COMMAND find "${CMAKE_BINARY_DIR}" -name "*.gcda" -delete
        COMMAND
            "${CMAKE_CTEST_COMMAND}"
            --test-dir "${CMAKE_BINARY_DIR}"
            --output-on-failure
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CLOADER_COVERAGE_OUTPUT_DIR}"
        COMMAND
            "${CLOADER_COVERAGE_GCOVR_EXECUTABLE}"
            --root "${PROJECT_SOURCE_DIR}"
            --object-directory "${CMAKE_BINARY_DIR}"
            --filter "${cloader_coverage_filter}"
            # Throw branches are the exception-propagation edges of calls
            # (allocation failures and the like); they cannot be covered by
            # tests, so they are left out of the report.
            --exclude-throw-branches
            --html-details "${CLOADER_COVERAGE_OUTPUT_DIR}/index.html"
            --html-title "cloader"
            --print-summary
        COMMENT "Generate coverage report"
        VERBATIM
    )

    # Make sure the test binaries are up to date before running the tests.
    if(TARGET cloader-tests)
        add_dependencies(coverage cloader-tests cloader_test_plugins)
    endif()
endfunction()
