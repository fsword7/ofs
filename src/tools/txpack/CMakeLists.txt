
set (txpack_src
    txpack.cpp
)

add_executable(txpack ${txpack_src})
target_link_libraries(txpack fmt::fmt ZLIB::ZLIB)