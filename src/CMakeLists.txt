add_definitions(-Wextra -Wall -std=c++14)
add_definitions(-g -DDEBUG)

include_directories(. ${libsInclude})

aux_source_directory(core coreSources)
aux_source_directory(. dotSources)
aux_source_directory(object objectSources)
aux_source_directory(world worldSources)
set(sources
    ${coreSources}
    ${dotSources}
    ${objectSources}
    ${worldSources}
)

find_package(Boost COMPONENTS any random polygon)
include_directories(${Boost_INCLUDE_DIR})

add_executable(btt ${sources})

target_link_libraries(btt m pugixml
    ${Boost_LIBRARIES}
    )
