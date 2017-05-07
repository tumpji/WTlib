file(GLOB sources *.cpp )
file(GLOB headers *.h   )


target_include_directories(wt PRIVATE .)
target_sources(wt PRIVATE ${sources})

add_subdirectory(3)
add_subdirectory(less3)
add_subdirectory(more3)

