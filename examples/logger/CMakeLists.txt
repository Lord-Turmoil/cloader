add_executable(logger main.cpp)
target_include_directories(logger PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(logger PRIVATE cloader::cloader)
install(
    TARGETS logger
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

macro(configure_library target_name)
    add_library(${target_name} SHARED ${target_name}.cpp)
    target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${target_name} PRIVATE cloader::cloader)
    install(
        TARGETS ${target_name}
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endmacro()

configure_library(StandardLogger)
configure_library(VerboseLogger)
