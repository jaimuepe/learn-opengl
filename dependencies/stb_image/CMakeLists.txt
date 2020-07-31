
project(stb-image)

add_library(stb-image STATIC src/stb_image.cpp)

target_include_directories(stb-image PRIVATE include)