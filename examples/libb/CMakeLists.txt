project(libb)

add_library(b SHARED libb.cpp)

target_include_directories(b PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../app")
target_link_libraries(b PRIVATE cloader::cloader)
