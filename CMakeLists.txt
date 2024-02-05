cmake_minimum_required( VERSION 3.1 )

project( Polar VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLAD_GL "" )

add_subdirectory( glfw )

option( POLAR-USE-GLFW-GLAD "Use GLAD from GLFW" ON )

if( POLAR-USE-GLFW-GLAD )
    include_directories("${GLFW_SOURCE_DIR}/deps")
    set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
endif()

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

set( POLAR-SRC
        main.cpp
        )

add_executable( Polar ${POLAR-SRC} ${GLAD_GL} )

# Include CMake Glad and customize its properties
include("Dependencies/CMakeGlad/CMakeLists.txt")
set(GLAD_GIT_SUBMODULE_PATH "${PROJECT_SOURCE_DIR}/Dependencies/Glad" CACHE INTERNAL "" FORCE) 
set(GLAD_LIBRARY_MODE    "STATIC" CACHE INTERNAL "" FORCE)
set(GLAD_API_GL_PROFILE  "CORE"   CACHE INTERNAL "" FORCE)
set(GLAD_API_GL_VERSION  "4.3"    CACHE INTERNAL "" FORCE)
set(GLAD_API_WGL_VERSION "1.0"    CACHE INTERNAL "" FORCE)
set(GLAD_API_EXTENSIONS  "WGL_ARB_create_context" CACHE INTERNAL "" FORCE)

# Create the "Glad" target (will generate source files)
target_create_glad()

# Add the "Glad" target as a dependency of your project
target_add_dependency_glad(MyProject)

target_link_libraries( Polar ${OPENGL_LIBRARIES} glfw )
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0")
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter )
    endif()
endif()