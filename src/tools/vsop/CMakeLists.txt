
set(vsop_list
	vsop87sph
)

foreach(vsop ${vsop_list})
	add_executable(${vsop} "${vsop}.cpp")
	target_link_libraries(${vsop} fmt::fmt)
endforeach()