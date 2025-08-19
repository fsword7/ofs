
set(OFS_CPP_SRCS
    control/hud/panel.cpp
    control/mfd/orbit.cpp
    control/mfd/panel.cpp
    control/mfd/surface.cpp
    control/gpanel.cpp
    control/info.cpp
    control/panel.cpp
    control/ppanel.cpp
    control/taskbar.cpp
    engine/vehicle/animation.cpp
    engine/vehicle/config.cpp
    engine/vehicle/mesh.cpp
    engine/vehicle/svehicle.cpp
    engine/vehicle/thruster.cpp
    engine/vehicle/vehicle.cpp
    engine/vehicle/wingctrl.cpp
    engine/base.cpp
    engine/celestial.cpp
    engine/dlgcam.cpp
    engine/engine.cpp
    # engine/frame.cpp
    engine/mesh.cpp
    engine/object.cpp
    # engine/observer.cpp
    engine/player.cpp
    engine/radio.cpp
    engine/rigidbody.cpp
    engine/view.cpp
    ephem/sol/earth/atmo.cpp
    ephem/sol/earth/nrlmsise00_data.cpp
    ephem/sol/earth/nrlmsise00_math.cpp
    ephem/sol/earth/nrlmsise00_p.cpp
    ephem/sol/earth/nrlmsise00.cpp
    ephem/sol/luna/elp82.cpp
    ephem/sol/luna/luna.cpp
    ephem/sol/mars/atmo.cpp
    ephem/sol/venus/atmo.cpp
    ephem/vsop87/earth.cpp
    ephem/vsop87/jupiter.cpp
    ephem/vsop87/mars.cpp
    ephem/vsop87/mercury.cpp
    ephem/vsop87/neptune.cpp
    ephem/vsop87/saturn.cpp
    ephem/vsop87/sol.cpp
    ephem/vsop87/uranus.cpp
    ephem/vsop87/venus.cpp
    ephem/vsop87/vsop87.cpp
    # ephem/earth-p03lp.cpp
    ephem/elements.cpp
    ephem/elp-mpp02.cpp
    ephem/ephemeris.cpp
    # ephem/iau-wgccre.cpp
    ephem/orbit.cpp
    ephem/rotation.cpp
    ephem/spice.cpp
    main/app.cpp
    main/guimgr.cpp
    main/graphics.cpp
    main/main.cpp
    main/module.cpp
    main/ofsapi.cpp
    main/timedate.cpp
    # render/annotation.cpp
    # render/elevmgr.cpp
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
    universe/astro.cpp
    universe/atmo.cpp
    universe/body.cpp
    universe/celbody.cpp
    universe/constellations.cpp
    universe/elevmgr.cpp
    # universe/frame.cpp
    universe/psystem.cpp
    universe/star.cpp
    universe/starlib.cpp
    universe/startree.cpp
    universe/surfmgr.cpp
    # universe/system.cpp
    universe/universe.cpp
    utils/color.cpp
    utils/json.cpp
    utils/string.cpp
    utils/ztreemgr.cpp
    ${IMGUI_LIBRARY_DIR}/backends/imgui_impl_glfw.cpp
)

set(OFS_H_SRCS
    api/draw.h
    api/graphics.h
    api/module.h
    api/ofsapi.h
    api/vehicle.h
    control/hud/panel.h
    control/mfd/orbit.h
    control/mfd/panel.h
    control/mfd/surface.h
    control/gpanel.h
    control/panel.h
    control/ppanel.h
    control/taskbar.h
    engine/vehicle/svehicle.h
    engine/vehicle/vehicle.h
    engine/base.h
    engine/celestial.h
    engine/dlgcam.h
    engine/engine.h
    # engine/frame.h
    engine/mesh.h
    engine/object.h
    # engine/observer.h
    engine/player.h
    engine/radio.h
    engine/rigidbody.h
    engine/view.h
    ephem/sol/earth/atmo.h
    ephem/sol/earth/nrlmsise00_math.hpp
    ephem/sol/earth/nrlmsise00_p.hpp
    ephem/sol/earth/nrlmsise00.hpp
    ephem/sol/luna/elp82.h
    ephem/sol/luna/luna.h
    ephem/sol/mars/atmo.h
    ephem/sol/venus/atmo.h
    ephem/vsop87/earth.h
    ephem/vsop87/jupiter.h
    ephem/vsop87/mars.h
    ephem/vsop87/mercury.h
    ephem/vsop87/neptune.h
    ephem/vsop87/saturn.h
    ephem/vsop87/sol.h
    ephem/vsop87/uranus.h
    ephem/vsop87/venus.h
    ephem/vsop87/vsop87.h
    ephem/elements.h
    ephem/elp-mpp02.h
    ephem/ephemeris.h
    ephem/orbit.h
    ephem/rotation.h
    ephem/spice.h
    main/app.h
    main/core.h
    main/guimgr.h
    main/math.h
    main/timedate.h
    # render/elevmgr.h
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
    universe/astro.h
    universe/atmo.h
    universe/body.h
    universe/celbody.h
    universe/constellations.h
    universe/elevmgr.h
    # universe/frame.h
    universe/handle.h
    universe/psystem.h
    universe/star.h
    universe/starlib.h
    universe/startree.h
    universe/surfmgr.h
    # universe/system.h
    universe/universe.h
    utils/color.h
    utils/json.h
    utils/string.h
    utils/tree.h
    # utils/yaml.h
    utils/ztreemgr.h
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
    fmt::fmt
    ZLIB::ZLIB
    nlohmann_json::nlohmann_json
    imgui
    glfw
    OpenGL::GL
)
if (MINGW)
target_link_libraries(ofs
    libdl.a
)
endif ()
target_compile_definitions(ofs 
    PUBLIC GIT_COMMIT_ID="${GIT_COMMIT_ID}"
    PUBLIC OFS_HOME_DIR="${OFS_HOME_DIR}"
    PUBLIC OFS_LIBRARY_DIR="${OFS_LIBRARY_DIR}"
    PUBLIC OFS_LIB_VEHICLE_DIR="${OFS_INSTALL_VEHICLE_DIR}"
)

target_include_directories(ofs PUBLIC
    ${IMGUI_INCLUDE_DIR}
)

set_target_properties(ofs
    PROPERTIES
    ENABLE_EXPORTS 1
)

# installing Sol system files
add_subdirectory(ephem/sol)
add_subdirectory(vehicles/glider)

install(DIRECTORY data/
    DESTINATION ${OFS_INSTALL_HOME_DIR}
)

install(TARGETS ofs
    RUNTIME DESTINATION ${OFS_INSTALL_BIN_DIR}
)
if (MINGW)
    install(FILES $<TARGET_LINKER_FILE:ofs>
        DESTINATION ${OFS_INSTALL_LIB_DIR}
    )
endif ()
