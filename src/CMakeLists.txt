find_package(SDL2 REQUIRED)
find_package(SDL2_Image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)

set(OFS_CPP_SRCS
    engine/engine.cpp
    engine/object.cpp
    engine/player.cpp
    engine/rigidbody.cpp
    ephem/elp-mpp02.cpp
    ephem/orbit.cpp
    ephem/vsop87.cpp
    main/app.cpp
    main/date.cpp
    render/scene.cpp
    render/starcolors.cpp
    render/surface.cpp
    render/systems.cpp
    render/vbody.cpp
    render/vobject.cpp
    render/vstar.cpp
    render/vvessel.cpp
    render/ztreemgr.cpp
    universe/astro.cpp
    universe/body.cpp
    universe/frame.cpp
    universe/star.cpp
    universe/starlib.cpp
    universe/startree.cpp
    universe/system.cpp
    universe/universe.cpp
)

set(OFS_H_SRCS
    engine/engine.h
    engine/object.h
    engine/player.h
    engine/rigidbody.h
    ephem/elp-mpp02.h
    ephem/orbit.h
    ephem/vsop87.h
    main/sdl/app.h
    main/app.h
    main/core.h
    main/date.h
    main/math.h
    render/scene.h
    render/starcolors.h
    render/surface.h
    render/vbody.h
    render/vobject.h
    render/vstar.h
    render/vvessel.h
    render/ztreemgr.h
    universe/star.h
    universe/body.h
    universe/frame.h
    universe/handle.h
    universe/star.h
    universe/starlib.h
    universe/startree.h
    universe/system.h
    universe/universe.h
    utils/color.h
    utils/tree.h
)

set(OFSGL_CPP_SRCS
    main/sdl/maingl.cpp
    osd/gl/buffers.cpp
    osd/gl/context.cpp
    osd/gl/mesh.cpp
    osd/gl/shader.cpp
    osd/gl/texture.cpp
    render/gl/stars.cpp
)

set(OFSGL_H_SRCS
    osd/gl/buffers.h
    osd/gl/context.h
    osd/gl/dds.h
    osd/gl/mesh.h
    osd/gl/shader.h
    osd/gl/texture.h
    render/gl/stars.h
)

add_executable(ofs ${OFS_CPP_SRCS} ${OFSGL_CPP_SRCS} ${OFS_H_SRCS} ${OFSGL_H_SRCS})
target_link_libraries(ofs
    Freetype::Freetype
    SDL2::Main
    SDL2::Image
    GLEW::GLEW
    OpenGL::GL
    fmt::fmt
    ZLIB::ZLIB
)