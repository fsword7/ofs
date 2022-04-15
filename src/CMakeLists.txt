find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
#find_package(ZLIB REQUIRED)

set(OFS_CPP_SRCS
    main/app.cpp
)

set(OFS_H_SRCS
    main/app.h
    main/core.h
)

set(OFSGL_CPP_SRCS
    main/sdl/maingl.cpp
)

set(OFSGL_H_SRCS
)

add_executable(ofs ${OFS_CPP_SRCS} ${OFSGL_CPP_SRCS} ${OFS_H_SRCS} ${OFSGL_H_SRCS})
target_link_libraries(ofs
    Freetype::Freetype
    SDL2::Main
    GLEW::GLEW
    OpenGL::GL
    fmt::fmt
)