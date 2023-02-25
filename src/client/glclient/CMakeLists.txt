
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)

set(OGL_CPP_SRCS
    buffer.cpp
    # camera.cpp
    client.cpp
    constellations.cpp
    glad.cpp
    scene.cpp
    shader.cpp
    starcolors.cpp
    stars.cpp
    surface.cpp
    systems.cpp
    vbody.cpp
    vobject.cpp
    vstar.cpp
    vvessel.cpp
    ${IMGUI_LIBRARY_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_LIBRARY_DIR}/backends/imgui_impl_opengl3.cpp
)

set(OGL_H_SRCS
    buffer.h
    # camera.h
    client.h
    scene.h
    shader.h
    starcolors.h
    stars.h
    surface.h
    vbody.h
    vobject.h
    vstar.h
    vvessel.h
)

set (OGL_SHADERS
    shaders/body.fs
    shaders/body.vs
    shaders/line.fs
    shaders/line.vs
    shaders/point.fs
    shaders/point.vs
    shaders/star.fs
    shaders/star.vs
)

add_library(glclient SHARED ${OGL_CPP_SRCS} ${OGL_H_SRCS})

target_link_libraries(glclient
    Freetype::Freetype
    glfw
    imgui
    OpenGL::GL
    fmt::fmt
)
if (MINGW)
target_link_libraries(glclient
    ${OFS_LIB}
    libdl.a
)
endif()

target_include_directories(glclient PUBLIC
    ${IMGUI_INCLUDE_DIR}
)

set_target_properties(glclient
    PROPERTIES
    DESTINATION modules
)

install(TARGETS glclient
    DESTINATION ${OFS_INSTALL_PLUGIN_DIR}
)

install(FILES ${OGL_SHADERS}
    DESTINATION ${OFS_INSTALL_ROOT_DIR}/shaders/gl
)