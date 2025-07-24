cmake_minimum_required(VERSION 3.16)
project(ofs VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED YES)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
#find_package(lua REQUIRED)
find_package(nlohmann_json REQUIRED)

# Get git commit ID
execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_COMMIT_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Git commit ID: ${GIT_COMMIT_ID}")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (UNIX)
        set (CMAKE_INSTALL_PREFIX ~/.local)
    endif (UNIX)
    if (MINGW)
        set (CMAKE_INSTALL_PREFIX /m/ofs)
    endif (MINGW)
endif ()
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

if (UNIX)
    set(OFS_HOME_DIR ${CMAKE_INSTALL_PREFIX}/share/ofs)
    set(OFS_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/bin)
    set(OFS_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib/ofs)
endif (UNIX)
if (MINGW)
    set(OFS_HOME_DIR ${CMAKE_INSTALL_PREFIX})
    set(OFS_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/bin)
    set(OFS_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif (MINGW)

set(OFS_SOURCE_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(OFS_BINARY_HOME_DIR ${CMAKE_BINARY_DIR})
set(OFS_BINARY_LIB_DIR ${OFS_BINARY_HOME_DIR}/lib)
set(OFS_BINARY_STARTUP_DIR ${OFS_BINARY_LIB_DIR}/startup)
set(OFS_BINARY_PLUGIN_DIR ${OFS_BINARY_LIB_DIR}/plugin)

# path in relative to CMAKE_INSTALL_PREFIX
set(OFS_INSTALL_HOME_DIR ${OFS_HOME_DIR})
set(OFS_INSTALL_BIN_DIR ${OFS_BINARY_DIR})
set(OFS_INSTALL_LIB_DIR ${OFS_LIBRARY_DIR})
set(OFS_INSTALL_STARTUP_DIR ${OFS_INSTALL_LIB_DIR}/startup)
set(OFS_INSTALL_PLUGIN_DIR ${OFS_INSTALL_LIB_DIR}/plugin)
set(OFS_INSTALL_VEHICLE_DIR ${OFS_INSTALL_LIB_DIR}/vehicle)

set(OFS_LIB $<TARGET_LINKER_FILE:ofs>)

set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/externs)

set(OFS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)

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
add_subdirectory(src/tools/txedit)
add_subdirectory(src/tools/txpack)

# Compiling for client modules
add_subdirectory(src/client/glclient)
# add_subdirectory(src/client/vkclient)

# Compiling OFS server
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
