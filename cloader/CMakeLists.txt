set(cloader_source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(cloader_generated_include_dir "${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cloader/version.h.in"
    "${cloader_generated_include_dir}/cloader/version.h"
    @ONLY
)

file(GLOB_RECURSE cloader_headers CONFIGURE_DEPENDS
    "${cloader_source_dir}/include/*.h"
)

file(GLOB_RECURSE cloader_sources CONFIGURE_DEPENDS 
    "${cloader_source_dir}/src/*.cpp"
)

add_library(cloader ${cloader_sources} ${cloader_headers})
add_library(cloader::cloader ALIAS cloader)

cloader_configure_compiler(cloader)

target_include_directories(
    cloader
    PUBLIC
        "$<BUILD_INTERFACE:${cloader_source_dir}/include>"
        "$<BUILD_INTERFACE:${cloader_generated_include_dir}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "$<BUILD_INTERFACE:${cloader_source_dir}/src>"
)

set_target_properties(
    cloader
    PROPERTIES
        EXPORT_NAME cloader
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

install(
    DIRECTORY "${cloader_source_dir}/include/cloader"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
        PATTERN "*.h"
)
install(FILES "${cloader_generated_include_dir}/cloader/version.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cloader"
)
install(TARGETS cloader
    EXPORT cloader-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
