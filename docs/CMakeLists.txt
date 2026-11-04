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

set(cloader_docs_input
    "${CLOADER_PROJECT_SOURCE_DIR}/cloader/include"
    "${CLOADER_PROJECT_SOURCE_DIR}/cloader/src"
    "${CMAKE_BINARY_DIR}/cloader/include"
    "${CLOADER_PROJECT_SOURCE_DIR}/docs/index.md"
    "${CLOADER_PROJECT_SOURCE_DIR}/docs/examples.md"
)
string(JOIN " " cloader_docs_input ${cloader_docs_input})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

configure_file(
    "${CLOADER_PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
    "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    @ONLY
)

add_custom_target(
    docs
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    WORKING_DIRECTORY "${CLOADER_PROJECT_SOURCE_DIR}"
    DEPENDS "${CMAKE_BINARY_DIR}/docs/Doxyfile"
    COMMENT "Generate API documentation"
    VERBATIM
)
