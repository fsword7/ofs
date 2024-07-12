
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

set (txedit_src
    main.cpp
)

qt_add_executable(txedit ${txedit_src})
target_link_libraries(txedit PRIVATE Qt6::Widgets)