
file(GLOB sources *.cpp )
file(GLOB headers *.h   )

target_include_directories(wt PRIVATE .)
target_sources(wt PRIVATE ${sources})

add_subdirectory(barevna_transformace)
add_subdirectory(dekompozice)
add_subdirectory(entropicky_koder)
add_subdirectory(stat)
add_subdirectory(vlnka)
add_subdirectory(predictor)
add_subdirectory(threading)

