cmake_minimum_required(VERSION 3.16)
project(ofs VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED YES)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

# if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#     set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/bin" CACHE PATH "..." FORCE)
# endif()

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
#find_package(lua REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(yaml-cpp REQUIRED)

set(OFS_SOURCE_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(OFS_BINARY_ROOT_DIR ${CMAKE_BINARY_DIR})
set(OFS_BINARY_MODULE_DIR ${OFS_BINARY_ROOT_DIR}/modules)
set(OFS_BINARY_STARTUP_DIR ${OFS_BINARY_MODULE_DIR}/startup)
set(OFS_BINARY_PLUGIN_DIR ${OFS_BINARY_MODULE_DIR}/plugin)

# path in relative to CMAKE_INSTALL_PREFIX
set(OFS_INSTALL_ROOT_DIR "${CMAKE_BINARY_DIR}/ofs")
set(OFS_INSTALL_MODULE_DIR ${OFS_INSTALL_ROOT_DIR}/modules)
set(OFS_INSTALL_STARTUP_DIR ${OFS_INSTALL_MODULE_DIR}/startup)
set(OFS_INSTALL_PLUGIN_DIR ${OFS_INSTALL_MODULE_DIR}/plugin)

set(OFS_LIB $<TARGET_LINKER_FILE:ofs>)

set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/externs)

# ImGUI libraries

set(IMGUI_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/src/imgui")
set(IMGUI_INCLUDE_DIR "${IMGUI_LIBRARY_DIR}")

set(IMGUI_SOURCES
    "${IMGUI_LIBRARY_DIR}/imgui.cpp"
    "${IMGUI_LIBRARY_DIR}/imgui_demo.cpp"
    "${IMGUI_LIBRARY_DIR}/imgui_draw.cpp"
    "${IMGUI_LIBRARY_DIR}/imgui_tables.cpp"
    "${IMGUI_LIBRARY_DIR}/imgui_widgets.cpp"
)

set(IMGUI_HEADERS
    "${IMGUI_LIBRARY_DIR}/imgui.h"
)

add_library(imgui OBJECT
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
)

target_compile_definitions(imgui PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

target_include_directories(imgui PUBLIC
    "${IMGUI_INCLUDE_DIR}"
)

# end of ImGUI libraries

include_directories(src)

# Compiling for creating orbital data headers
add_subdirectory(src/tools/vsop87)
# add_subdirectory(src/tools/elp82b)

# Compiling for client modules
add_subdirectory(src/client/glclient)
# add_subdirectory(src/client/vkclient)

# Compiling OFS server
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
