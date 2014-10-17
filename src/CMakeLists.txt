add_definitions(-Wextra -Wall -std=c++14)
add_definitions(-g -DDEBUG)

include_directories(. ${pugixmlInclude})

aux_source_directory(core coreSources)
aux_source_directory(. dotSources)
aux_source_directory(object objectSources)
set(sources
    ${coreSources}
    ${dotSources}
    ${objectSources}
)

#find_package(Boost COMPONENTS )
#include_directories(${Boost_INCLUDE_DIR})

add_executable(btt ${sources})

target_link_libraries(btt m pugixml
    #${Boost_LIBRARIES}
    )
