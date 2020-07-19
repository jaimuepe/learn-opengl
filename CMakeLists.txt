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

include_directories("dependencies/glfw/include")
include_directories("dependencies/glad/include")
include_directories("dependencies/stb_image/include")

include_directories("src/common")

link_libraries(${LIBS})

set(CHAPTERS
    "1.getting-started")

set("1.getting-started"
    "1.1-hello-window"
    "2.1-hello-triangle"
    "2.2-hello-triangle-indexed"
    "2.3-hello-triangle-exercise-1"
    "2.4-hello-triangle-exercise-2"
    "2.5-hello-triangle-exercise-3"
    "3.1-shaders-uniform"
    "3.2-shaders-interpolation"
    "3.3-shaders-class"
    "3.4-shaders-exercise-1"
    "3.5-shaders-exercise-2"
    "3.6-shaders-exercise-3"
    "4.1-textures"
    "4.2-textures-combined"
    "4.3-textures-exercise-1"
    "4.4-textures-exercise-2"
    "4.5-textures-exercise-3"
    "4.6-textures-exercise-4")

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

set (TEXTURES
    "container.jpg"
    "wall.jpg"
    "awesomeface.png")

foreach (TEXTURE ${TEXTURES})
    message("Copying texture: ${TEXTURE} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures")
    configure_file(
        "resources/textures/${TEXTURE}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/${TEXTURE}"
        COPYONLY
        )
endforeach(TEXTURE)