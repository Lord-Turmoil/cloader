add_executable(cloader-example main.cpp)

cloader_configure_compiler(cloader-example)
target_link_libraries(cloader-example PRIVATE cloader::cloader)

install(
    TARGETS cloader-example
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
