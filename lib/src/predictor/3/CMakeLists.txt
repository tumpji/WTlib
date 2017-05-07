file(GLOB sources *.cpp )
file(GLOB headers *.h   )


target_include_directories(wt PRIVATE .)
target_sources(wt PRIVATE ${sources})
