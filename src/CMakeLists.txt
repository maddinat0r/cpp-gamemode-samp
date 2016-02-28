include(AMXConfig)
include(AddSAMPPlugin)

set_property(GLOBAL PROPERTY _DAWN_SOURCE_FILES "")
macro(add_sources)
  file(RELATIVE_PATH _rel_path "${CMAKE_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
  
  foreach(_src_file ${ARGN})
    set_property(GLOBAL APPEND PROPERTY _DAWN_SOURCE_FILES "${_rel_path}/${_src_file}")
  endforeach()
endmacro(add_sources)


set(DAWN_MODULES 
	MCore
	MySQL
	System
	Textdraw
	Time
	Plugin
	Area
	Player
	Vehicle
	House
	Company
)

set(_CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/config_headers")
configure_file(
	"version.hpp.in"
	"${_CONFIG_INCLUDE_DIR}/version.hpp"
	@ONLY
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${_CONFIG_INCLUDE_DIR})


foreach(module ${DAWN_MODULES})
	add_subdirectory(${module})
endforeach(module)

get_property(_SOURCEFILES GLOBAL PROPERTY _DAWN_SOURCE_FILES)

# add VS filters
foreach(src_file ${_SOURCEFILES})
  string(REGEX MATCH "(.+)/.+\\..+" _unused ${src_file})
  set(_rel_path ${CMAKE_MATCH_1})
  string(REPLACE "/" "\\" _group_name ${_rel_path})
  source_group(${_group_name} FILES ${src_file})
endforeach()

add_samp_plugin(dawn
	${_SOURCEFILES}
)

# link dependencies
target_link_libraries(dawn sampgdk4d ${MYSQLCAPI_LIBRARY} ${YAML_CPP_LIBRARY})# ${CRYPTOPP_LIBRARY})

install(TARGETS dawn DESTINATION "plugins/")
