
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Freetype REQUIRED)

set(OGL_CPP_SRCS
    buffer.cpp
    client.cpp
    scene.cpp
    surface.cpp
    vbody.cpp
    vobject.cpp
    vstar.cpp
    vvessel.cpp
)

set(OGL_H_SRCS
    buffer.h
    client.h
    scene.h
    surface.h
    vbody.h
    vobject.h
    vstar.h
    vvessel.h
)

add_library(glclient SHARED ${OGL_CPP_SRCS} ${OGL_H_SRCS})

target_link_libraries(glclient
    Freetype::Freetype
    SDL2::Main
    SDL2::Image
    GLEW::GLEW
    OpenGL::GL
    Eigen3::Eigen
)
if (MINGW)
target_link_libraries(glclient
    ${OFS_LIB}
    libdl.a
)
endif()

set_target_properties(glclient
    PROPERTIES
    DESTINATION modules
)

install(TARGETS glclient
    DESTINATION ${OFS_INSTALL_PLUGIN_DIR}
)