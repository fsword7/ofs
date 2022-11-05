find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
#find_package(lua REQUIRED)

set(OFS_CPP_SRCS
    engine/camera.cpp
    engine/engine.cpp
    engine/headup.cpp
    engine/mfdpanel.cpp
    engine/object.cpp
    engine/panel.cpp
    # engine/player.cpp
    engine/rigidbody.cpp
    engine/supervessel.cpp
    engine/vessel.cpp
    engine/view.cpp
    ephem/earth/atmo.cpp
    ephem/earth/nrlmsise00_data.cpp
    ephem/earth/nrlmsise00_math.cpp
    ephem/earth/nrlmsise00_p.cpp
    ephem/earth/nrlmsise00.cpp
    ephem/moon/elp82.cpp
    ephem/earth-p03lp.cpp
    ephem/elements.cpp
    ephem/elp-mpp02.cpp
    ephem/iau-wgccre.cpp
    ephem/orbit.cpp
    ephem/rotation.cpp
    ephem/vsop87.cpp
    ephem/vsop87a.cpp
    main/app.cpp
    # main/date.cpp
    main/graphics.cpp
    main/main.cpp
    main/module.cpp
    main/ofsapi.cpp
    main/timedate.cpp
    # render/annotation.cpp
    # render/elevmgr.cpp
    # render/hudpanel.cpp
    # render/lights.cpp
    # render/mesh.cpp
    # render/loader.cpp
    # render/planetgrid.cpp
    # render/overlay.cpp
    # render/refmark.cpp
    # render/scene.cpp
    # render/starcolors.cpp
    # render/surface.cpp
    # render/systems.cpp
    # render/vbody.cpp
    # render/vobject.cpp
    # render/vstar.cpp
    # render/vvessel.cpp
    # render/ztreemgr.cpp
    scripts/parser.cpp
    universe/astro.cpp
    universe/atmo.cpp
    universe/body.cpp
    universe/constellations.cpp
    universe/frame.cpp
    universe/star.cpp
    universe/starlib.cpp
    universe/startree.cpp
    universe/surface.cpp
    universe/system.cpp
    universe/universe.cpp
    utils/logger.cpp
    utils/string.cpp
)

set(OFS_H_SRCS
    api/draw.h
    api/graphics.h
    api/module.h
    api/ofsapi.h
    engine/camera.h
    engine/engine.h
    engine/mfdpanel.h
    engine/object.h
    engine/panel.h
    # engine/player.h
    engine/rigidbody.h
    engine/supervessel.h
    engine/vessel.h
    engine/view.h
    ephem/earth/atmo.h
    ephem/earth/nrlmsise00_math.hpp
    ephem/earth/nrlmsise00_p.hpp
    ephem/earth/nrlmsise00.hpp
    ephem/moon/elp82.h
    ephem/elements.h
    ephem/elp-mpp02.h
    ephem/orbit.h
    ephem/rotation.h
    ephem/vsop87.h
    ephem/vsop87a.h
    # main/sdl/app.h
    main/app.h
    main/core.h
    # main/date.h
    main/math.h
    main/timedate.h
    # render/elevmgr.h
    # render/hudpanel.h
    # render/mesh.h
    # render/planetgrid.h
    # render/overlay.h
    # render/refmark.h
    # render/scene.h
    # render/starcolors.h
    # render/surface.h
    # render/vbody.h
    # render/vobject.h
    # render/vstar.h
    # render/vvessel.h
    # render/ztreemgr.h
    scripts/parser.h
    universe/astro.h
    universe/atmo.h
    universe/body.h
    universe/constellations.h
    universe/frame.h
    universe/handle.h
    universe/star.h
    universe/starlib.h
    universe/startree.h
    universe/surface.h
    universe/system.h
    universe/universe.h
    utils/color.h
    utils/logger.h
    utils/string.h
    utils/tree.h
)

set(OFSGL_CPP_SRCS
    # main/sdl/maingl.cpp
    # osd/gl/buffers.cpp
    # osd/gl/context.cpp
    # osd/gl/mesh.cpp
    # osd/gl/shader.cpp
    # osd/gl/texture.cpp
    # render/gl/constellations.cpp
    # render/gl/fonts.cpp
    # render/gl/stars.cpp
)

set(OFSGL_H_SRCS
    # osd/gl/buffers.h
    # osd/gl/context.h
    # osd/gl/dds.h
    # osd/gl/mesh.h
    # osd/gl/shader.h
    # osd/gl/texture.h
    # render/gl/fonts.h
    # render/gl/stars.h
)

add_executable(ofs ${OFS_CPP_SRCS} ${OFSGL_CPP_SRCS} ${OFS_H_SRCS} ${OFSGL_H_SRCS})
target_link_libraries(ofs
    Freetype::Freetype
    SDL2::Main
    SDL2::Image
    GLEW::GLEW
    OpenGL::GL
    Eigen3::Eigen
    fmt::fmt
    ZLIB::ZLIB
    libdl.a
)

set_target_properties(ofs
    PROPERTIES
    ENABLE_EXPORTS 1
)

install(TARGETS ofs
    RUNTIME
    DESTINATION ${OFS_INSTALL_ROOT_DIR}
)

install(FILES $<TARGET_LINKER_FILE:ofs>
    DESTINATION ${OFS_INSTALL_ROOT_DIR}/lib
)

add_subdirectory(client/glclient)
# add_subdirectory(client/vkclient)