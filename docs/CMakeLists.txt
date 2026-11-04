find_package(Doxygen QUIET COMPONENTS dot)

if(NOT Doxygen_FOUND)
    message(FATAL_ERROR "CLOADER_BUILD_DOCS is ON but Doxygen was not found. ")
endif()

if(Doxygen_dot_FOUND)
    set(cloader_docs_have_dot YES)
else()
    set(cloader_docs_have_dot NO)
endif()

message(STATUS "Doxygen ${DOXYGEN_VERSION} found - docs target enabled (dot: ${cloader_docs_have_dot})")

# Where the generated documentation ends up. Defaults to <build>/docs, but can
# be overridden (the docs CMake preset points it at <build>/docs so the output
# stays the same no matter how the build directory is named).
set(CLOADER_DOCS_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs" CACHE PATH "Output directory for the generated documentation")

set(cloader_docs_input
    "${PROJECT_SOURCE_DIR}/cloader/include"
    "${PROJECT_SOURCE_DIR}/cloader/src"
    "${CMAKE_BINARY_DIR}/cloader/include"
    "${PROJECT_SOURCE_DIR}/docs/index.md"
    "${PROJECT_SOURCE_DIR}/docs/examples.md"
)
string(JOIN " " cloader_docs_input ${cloader_docs_input})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

configure_file(
    "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
    "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    @ONLY
)

add_custom_target(
    docs
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    DEPENDS "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    COMMENT "Generate API documentation"
    VERBATIM
)
