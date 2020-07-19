
project(stb-image)

add_library(stb-image src/stb_image.cpp)

target_include_directories(stb-image PRIVATE include)