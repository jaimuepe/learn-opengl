cmake_minimum_required(VERSION 3.6)

set (CMAKE_CXX_STANDARD 17)

project(learn-opengl)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(LIBS glfw glad stb-image)

if(WIN32)
set(LIBS ${LIBS} Shlwapi pathcch)
endif(WIN32)

add_subdirectory("dependencies/glfw")
add_subdirectory("dependencies/glad")
add_subdirectory("dependencies/stb_image")

include_directories("dependencies/glad/include")
include_directories("dependencies/glfw/include")
include_directories("dependencies/glm/")
include_directories("dependencies/stb_image/include")

include_directories("src/common")

link_libraries(${LIBS})

set(CHAPTERS
    "1.getting-started"
    "2.lighting")

set("1.getting-started"
    "1.1-hello-window"
    "1.2.1-hello-triangle"
    "1.2.2-hello-triangle-indexed"
    "1.2.3-hello-triangle-exercise-1"
    "1.2.4-hello-triangle-exercise-2"
    "1.2.5-hello-triangle-exercise-3"
    "1.3.1-shaders-uniform"
    "1.3.2-shaders-interpolation"
    "1.3.3-shaders-class"
    "1.3.4-shaders-exercise-1"
    "1.3.5-shaders-exercise-2"
    "1.3.6-shaders-exercise-3"
    "1.4.1-textures"
    "1.4.2-textures-combined"
    "1.4.3-textures-exercise-1"
    "1.4.4-textures-exercise-2"
    "1.4.5-textures-exercise-3"
    "1.4.6-textures-exercise-4"
    "1.5.1-transformations"
    "1.5.2-transformations-exercise-1"
    "1.5.3-transformations-exercise-2"
    "1.6.1-coordinate-systems"
    "1.6.2-coordinate-systems-depth"
    "1.6.3-coordinate-systems-multiple"
    "1.6.4-coordinate-systems-exercise-1"
    "1.6.5-coordinate-systems-exercise-2"
    "1.6.6-coordinate-systems-exercise-3"
    "1.7.1-camera-circle"
    "1.7.2-camera-keyboard-dt"
    "1.7.3-camera-class"
    "1.7.4-camera-exercise-1")

set("2.lighting"
    "2.1-colors"
    "2.2.1-basic-lighting-diffuse"
    "2.2.2-basic-lighting-specular"
    "2.2.3-basic-lighting-exercise-1"
    "2.2.4-basic-lighting-exercise-2"
    "2.2.5-basic-lighting-exercise-3"
    "2.3.1-materials"
    "2.3.2-materials-exercise-1"
    "2.3.3-materials-exercise-2"
    "2.4.1-lighting-maps-diffuse-map"
    "2.4.2-lighting-maps-specular-map"
    "2.4.3-lighting-maps-exercise-2"
    "2.4.4-lighting-maps-exercise-3"
    "2.4.5-lighting-maps-exercise-4")
    
foreach (CHAPTER ${CHAPTERS})

    foreach(DEMO ${${CHAPTER}})

        add_executable(${DEMO} "src/${CHAPTER}/${DEMO}/${DEMO}.cpp")
        set_target_properties(${DEMO} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/${DEMO}")
        
        file(
            GLOB SHADERS
            "src/${CHAPTER}/${DEMO}/*.vs"
            "src/${CHAPTER}/${DEMO}/*.fs"
        )

        foreach (SHADER ${SHADERS})
            message("Copying shader: ${SHADER} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CHAPTER}/${DEMO}")
            add_custom_command(
                TARGET ${DEMO} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy 
                ${SHADER} 
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CHAPTER}/${DEMO})
        endforeach(SHADER)

    endforeach(DEMO)
endforeach(CHAPTER)

file(
    GLOB TEXTURES
    "resources/textures/*.jpg"
    "resources/textures/*.png"
)

foreach (TEXTURE ${TEXTURES})
    get_filename_component(TEXNAME ${TEXTURE} NAME)
    message("Copying texture: ${TEXNAME} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures")
    configure_file(
        "${TEXTURE}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/${TEXNAME}"
        COPYONLY
        )
endforeach(TEXTURE)