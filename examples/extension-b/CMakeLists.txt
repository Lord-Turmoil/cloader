project("extension-b")

add_library(extension-b SHARED extension-b.cpp)

cloader_configure_compiler(extension-b)
target_include_directories(extension-b PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../app")
target_link_libraries(extension-b PRIVATE cloader::cloader)