# The host application links the library; the plugins only need the headers.
add_executable(logger main.cpp)
target_include_directories(logger PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(logger PRIVATE cloader::cloader)

function(cloader_add_logger_plugin target_name)
    add_library(${target_name} SHARED ${target_name}.cpp)
    target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${target_name} PRIVATE cloader::headers)
endfunction()

cloader_add_logger_plugin(StandardLogger)
cloader_add_logger_plugin(VerboseLogger)

if(CLOADER_INSTALL)
    install(
        TARGETS logger
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT cloader_Examples
    )
    install(
        TARGETS StandardLogger VerboseLogger
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT cloader_Examples
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT cloader_Examples
    )
endif()
