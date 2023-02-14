
set(vsop_list
	vsop87
)

set(planet_list
	mercury
	venus
	earth
	mars
	jupiter
	saturn
	uranus
	neptune
)

foreach(vsop ${vsop_list})
	add_executable(${vsop} "${vsop}.cpp")
	target_link_libraries(${vsop} fmt::fmt)
endforeach()