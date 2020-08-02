cmake_minimum_required(VERSION 3.17)

project(learn-opengl)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)

set (CMAKE_CXX_STANDARD 11)
add_subdirectory("dependencies/assimp")

set (CMAKE_CXX_STANDARD 17)
add_subdirectory("dependencies/glad")
add_subdirectory("dependencies/glfw")
add_subdirectory("dependencies/glm")
add_subdirectory("dependencies/stb_image")

set(LIBS 
    glfw
    assimp
    glad 
    stb-image 
)

if(WIN32)
    set(LIBS ${LIBS} Shlwapi pathcch)
endif(WIN32)

set (MY_HEADERS
    "src/shared/filesystem.h"
    "src/shared/flycamera.h"
    "src/shared/mesh.h"
    "src/shared/model.h"
    "src/shared/pointlight.h"
    "src/shared/resources.h"
    "src/shared/shader.h"
    "src/shared/texture.h"
    "src/shared/vertex.h")

set(CHAPTERS
    "1.getting-started"
    "2.lighting"
    "3.model-loading"
    "4.advanced-opengl"
    "5.advanced-lighting"
    )

set("1.getting-started"
    "1.1.1-hello-window"
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
    "2.1.1-colors"
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
    "2.4.5-lighting-maps-exercise-4"
    "2.5.1-light-casters-directional"
    "2.5.2-light-casters-point"
    "2.5.3-light-casters-spot"
    "2.5.4-light-casters-spot-soft"
    "2.6.1-multiple-lights"
    "2.6.2-multiple-lights-exercise-1")

set("3.model-loading"
    "3.1-model-loading")

set("4.advanced-opengl"
    "4.1.1-depth-testing"
    "4.1.2-depth-testing-view"
    "4.2.1-stencil-testing"
    "4.3.1-blending-discard"
    "4.3.2-blending-sort"
    "4.4.1-face-culling"
    "4.5.1-framebuffers"
    "4.5.2-framebuffers-postprocessing"
    "4.5.3-framebuffers-exercise-1"
    "4.6.1-cubemaps-skybox"
    "4.6.2-cubemaps-environment-mapping"
    "4.8.1-advanced-glsl-ubo"
    "4.9.1-geometry-shader-houses"
    "4.9.2-geometry-shader-exploding"
    "4.9.3-geometry-shader-normals"
    "4.10.1-instancing-quads"
    "4.10.2-asteroids"
    "4.10.3-asteroids-instanced"
    "4.11.1-anti-aliasing-msaa"
    "4.11.2-anti-aliasing-offscreen")

set("5.advanced-lighting"
    "5.1.1.blinn-phong")

include_directories(
    dependencies/assimp/include
    dependencies/glad/include
    dependencies/glfw/include
    dependencies/glm
    dependencies/stb_image/include
    src/shared
)

foreach (CHAPTER ${CHAPTERS})

    foreach(DEMO ${${CHAPTER}})

        add_executable(${DEMO} "src/${CHAPTER}/${DEMO}/${DEMO}.cpp")
        set_target_properties(${DEMO} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/${DEMO}")

        target_link_libraries(${DEMO} ${LIBS})
        
        file(
            GLOB SHADERS
            "src/${CHAPTER}/${DEMO}/*.vs"
            "src/${CHAPTER}/${DEMO}/*.fs"
            "src/${CHAPTER}/${DEMO}/*.gs"
        )

        foreach (SHADER ${SHADERS})
            message("Copying shader: ${SHADER} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CHAPTER}/${DEMO}")
            add_custom_command(
                TARGET ${DEMO} PRE_BUILD
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

set (MODELS
    "backpack"
    "rock"
    "planet"
)

set (CUBEMAPS
    "coast")

foreach (CUBEMAP ${CUBEMAPS})

    file(
        GLOB FILES
        "resources/cubemaps/${CUBEMAP}/*.*"
    )
    
    message("Copying cubemap: ${CUBEMAP} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/cubemaps/${CUBEMAP}")
    foreach (FILE ${FILES}) 
        get_filename_component(FILENAME ${FILE} NAME)
        configure_file(
            "resources/cubemaps/${CUBEMAP}/${FILENAME}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/cubemaps/${CUBEMAP}/${FILENAME}"
            COPYONLY
        )
    endforeach(FILE)

endforeach(CUBEMAP)

foreach (MODEL ${MODELS})

    file(
        GLOB FILES
        "resources/models/${MODEL}/*.*"
    )

    message("Copying model: ${MODEL} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/models/${MODEL}")
    foreach (FILE ${FILES}) 
        get_filename_component(FILENAME ${FILE} NAME)
        configure_file(
            "resources/models/${MODEL}/${FILENAME}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/models/${MODEL}/${FILENAME}"
            COPYONLY
        )
    endforeach(FILE)

endforeach(MODEL)

foreach (TEXTURE ${TEXTURES})
    get_filename_component(TEXNAME ${TEXTURE} NAME)
    message("Copying texture: ${TEXNAME} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures")
    configure_file(
        "${TEXTURE}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/${TEXNAME}"
        COPYONLY
        )
endforeach(TEXTURE)