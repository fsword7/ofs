
set(stardb_list
	buildstars
)

foreach(stardb ${stardb_list})
	add_executable(${stardb} "${stardb}.cpp")
	target_link_libraries(${stardb} fmt::fmt)
endforeach()