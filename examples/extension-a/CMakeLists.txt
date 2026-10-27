project("extension-a")

add_library(extension-a SHARED extension-a.cpp)

cloader_configure_compiler(extension-a)
target_include_directories(extension-a PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../app")
target_link_libraries(extension-a PRIVATE cloader::cloader)
