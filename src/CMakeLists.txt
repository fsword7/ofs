find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
#find_package(ZLIB REQUIRED)

set(OFS_CPP_SRCS
    engine/engine.cpp
    engine/object.cpp
    engine/player.cpp
    main/app.cpp
    render/scene.cpp
    render/surface.cpp
    render/vbody.cpp
    render/vobject.cpp
    render/vstar.cpp
    render/vvessel.cpp
    universe/astro.cpp
    universe/body.cpp
    universe/star.cpp
    universe/starlib.cpp
    universe/startree.cpp
    universe/universe.cpp
)

set(OFS_H_SRCS
    engine/engine.h
    engine/object.h
    engine/player.h
    main/sdl/app.h
    main/app.h
    main/core.h
    main/math.h
    render/scene.h
    render/surface.h
    render/vbody.h
    render/vobject.h
    render/vstar.h
    render/vvessel.h
    universe/star.h
    universe/body.h
    universe/handle.h
    universe/star.h
    universe/starlib.h
    universe/startree.h
    universe/universe.h
    utils/tree.h
)

set(OFSGL_CPP_SRCS
    main/sdl/maingl.cpp
    osd/gl/context.cpp
    osd/gl/mesh.cpp
)

set(OFSGL_H_SRCS
    osd/gl/context.h
    osd/gl/mesh.h
)

add_executable(ofs ${OFS_CPP_SRCS} ${OFSGL_CPP_SRCS} ${OFS_H_SRCS} ${OFSGL_H_SRCS})
target_link_libraries(ofs
    Freetype::Freetype
    SDL2::Main
    GLEW::GLEW
    OpenGL::GL
    fmt::fmt
)