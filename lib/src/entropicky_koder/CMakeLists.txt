
file(GLOB sources *.cpp )
file(GLOB headers *.h   )

target_include_directories(wt PRIVATE .)
target_sources(wt PRIVATE ${sources})

add_subdirectory(ebcot)
add_subdirectory(nic)
add_subdirectory(aritmeticky)
