CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(projbc)

SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")

SET(CImg_header ${CMAKE_SOURCE_DIR}/thirdparty/CImg-2.0.0_pre021617)




MACRO(ADD_COMPILE_ARG  X)
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${X}" )
ENDMACRO(ADD_COMPILE_ARG)

MACRO(ADD_LINK_ARG  X)
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${X}" )
ENDMACRO(ADD_LINK_ARG)

MACRO(ADD_ARG X)
    ADD_COMPILE_ARG("${X}")
    ADD_LINK_ARG("${X}")
ENDMACRO(ADD_ARG)


#set(debug ON)
#set(profile ON)
set(release ON)

ADD_COMPILE_ARG(-std=c++17)

if (debug) 
    message( INFO "DEBUG ...")
    ADD_ARG(-O0)
    ADD_ARG(-g3)
    ADD_ARG(-ggdb)

    ADD_ARG(-Werror)
    ADD_ARG(-Wextra)
elseif (profile)
    message( INFO "PROFILOVANI...")
    ADD_ARG(-DNDEBUG)
    ADD_ARG(-O3)

    ADD_ARG(-pg)
elseif(release)
    #ADD_ARG(-DNDEBUG)
    ADD_ARG(-O3)
else()
    message(FATAL_ERROR "Neznam jakym zpusobem mam kompilovat")
endif()


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/inc)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)




ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(demo)
