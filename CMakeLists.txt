cmake_minimum_required(VERSION 3.6)

project(learn-opengl)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(LIBS glfw glad)

if(WIN32)
# find_library(SHLWAPI Shlwapi.lib) 
# find_library(PATHCCH pathcch.lib) 
set(LIBS ${LIBS} Shlwapi pathcch)
endif(WIN32)

message(" ---------------> ${LIBS}")

add_subdirectory(glfw)
add_subdirectory(glad)

include_directories("glfw/include")
include_directories("glad/include")

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
    "3.6-shaders-exercise-3")

add_custom_target(shader-target ALL DEPENDS ${SHADERS})

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
                TARGET shader-target PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy 
                ${SHADER} 
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CHAPTER}/${DEMO})
        endforeach(SHADER)

    endforeach(DEMO)
endforeach(CHAPTER)

