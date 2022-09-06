
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Freetype REQUIRED)

set(OGL_CPP_SRCS
    glclient.cpp
    scene.cpp
)

set(OGL_H_SRCS
    glclient.h
    scene.h
)

add_library(glclient SHARED ${OGL_CPP_SRCS} ${OGL_H_SRCS})

target_link_libraries(glclient
    ${OFS_LIB}
    Freetype::Freetype
    SDL2::Main
    SDL2::Image
    GLEW::GLEW
    OpenGL::GL
    Eigen3::Eigen
    libdl.a
)

set_target_properties(glclient
    PROPERTIES
    DESTINATION modules
)

install(TARGETS glclient
    DESTINATION ${OFS_INSTALL_PLUGIN_DIR}
)