add_subdirectory(glm)
add_subdirectory(pugixml)

set(libsInclude ${glmInclude} ${pugixmlInclude}
    CACHE INTERNAL "libs include directories")
