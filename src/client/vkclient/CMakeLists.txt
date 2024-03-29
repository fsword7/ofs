
find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)
# find_package(Fontconfig)
# find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)

set(VK_CPP_SRCS
    # buffer.cpp
    # camera.cpp
    client.cpp
    # constellations.cpp
    # elevmgr.cpp
    # fonts.cpp
    # glad.cpp
    # loader.cpp
    # lights.cpp
    # overlay.cpp
    # ringmgr.cpp
    # scene.cpp
    # shader.cpp
    # skpad.cpp
    # starcolors.cpp
    # stars.cpp
    # surface.cpp
    # systems.cpp
    # texmgr.cpp
    # vbody.cpp
    # vmesh.cpp
    # vobject.cpp
    # vstar.cpp
    # vvessel.cpp
    # ztreemgr.cpp
    # ${IMGUI_LIBRARY_DIR}/backends/imgui_impl_glfw.cpp
    # ${IMGUI_LIBRARY_DIR}/backends/imgui_impl_opengl3.cpp
)

set(VK_H_SRCS
    # buffer.h
    # camera.h
    client.h
    # fonts.h
    # lights.h
    # overlay.h
    # ringmgr.h
    # scene.h
    # shader.h
    # skpad.h
    # starcolors.h
    # stars.h
    # surface.h
    # texmgr.h
    # vbody.h
    # vmesh.h
    # vobject.h
    # vstar.h
    # vvessel.h
    # ztreemgr.h
)

set (VK_SHADERS
    # shaders/lib/logdepth.glsl
    # shaders/lib/snoise3.glsl
    # shaders/lib/snoise4.glsl
    # shaders/body.fs
    # shaders/body.vs
    # shaders/corona.fs
    # shaders/corona.vs
    # shaders/line.fs
    # shaders/line.vs
    # shaders/point.fs
    # shaders/point.vs
    # shaders/star.fs
    # shaders/star.vs
)

set (SOIL2_SRCS
    # SOIL2/src/SOIL2/image_DXT.c
    # SOIL2/src/SOIL2/image_helper.c
    # SOIL2/src/SOIL2/SOIL2.c 
    # SOIL2/src/SOIL2/wfETC.c
)

set (NANOVG_SRCS
    # nanovg/src/nanovg.c
)

# add_library(vkclient SHARED ${VK_CPP_SRCS} ${SOIL2_SRCS} ${NANOVG_SRCS} ${VK_H_SRCS})

target_link_libraries(vkclient
    # Freetype::Freetype
    # Fontconfig::Fontconfig
    glfw
    # imgui
    # OpenGL::GL
    fmt::fmt
    # ZLIB::ZLIB
)
if (MINGW)
target_link_libraries(vkclient
    ${OFS_LIB}
    libdl.a
)
endif()

target_compile_definitions(vkclient PRIVATE
    NVG_NO_STB=1 FONS_USE_FREETYPE=1
)

target_include_directories(vkclient PUBLIC
    # ${IMGUI_INCLUDE_DIR}
    # SOIL2/src
    .
)

set_target_properties(vkclient
    PROPERTIES
    DESTINATION modules
)

install(TARGETS vkclient
    DESTINATION ${OFS_INSTALL_PLUGIN_DIR}
)

# install(FILES ${VK_SHADERS}
#     DESTINATION ${OFS_INSTALL_ROOT_DIR}/shaders/gl
# )

# install(FILES ${VK_SHADERS}
#     DESTINATION ${OFS_INSTALL_ROOT_DIR}/shaders/gl/lib
# )