
file(GLOB sources *.cpp )
file(GLOB headers *.h   )

target_include_directories(wt PRIVATE .)
target_sources(wt PRIVATE ${sources})

add_subdirectory(mq_ffmpeg)
add_subdirectory(ebcot_ffmpeg)
