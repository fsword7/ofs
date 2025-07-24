# find_package(glfw3 3.3 REQUIRED)
# find_package(OpenGL REQUIRED)
# find_package(Fontconfig)
# find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)

set(SG_CPP_SRCS
    glider.cpp
    module.cpp
)

set(SG_H_SRCS
    glider.h
)

add_library(glider SHARED ${SG_CPP_SRCS} ${SG_H_SRCS})

target_link_libraries(glider
    fmt::fmt
)
if (MINGW)
target_link_libraries(glider
    ${OFS_LIB}
    libdl.a
)
endif()

target_compile_definitions(glider
    PRIVATE NVG_NO_STB=1 FONS_USE_FREETYPE=1
    PUBLIC GIT_COMMIT_ID="${GIT_COMMIT_ID}"
    PUBLIC OFS_HOME_DIR="${OFS_HOME_DIR}"
    PUBLIC OFS_LIBRARY_DIR="${OFS_INSTALL_VEHICLE_DIR}"
)

target_include_directories(glider PUBLIC
    .
)

set_target_properties(glclient
    PROPERTIES
    DESTINATION modules
)

install(TARGETS glider
    DESTINATION ${OFS_INSTALL_VEHICLE_DIR}
)
