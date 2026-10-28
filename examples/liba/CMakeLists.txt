project(liba)

add_library(a SHARED liba.cpp)

target_include_directories(a PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../app")
target_link_libraries(a PRIVATE cloader::cloader)
