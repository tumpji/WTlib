PROJECT(wt)

file(GLOB sources *.cpp )
file(GLOB headers *.h   )


MESSAGE( STATUS ${${PROJECT_NAME}_sources} )

# vytvoreni knihovny 
ADD_LIBRARY(
	${PROJECT_NAME}
	STATIC
	${headers}
	${sources}
    )

# vkladani .h
TARGET_INCLUDE_DIRECTORIES(
	${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	 PRIVATE 
		 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	)



#[[
ADD_LIBRARY( wtlib-internal STATIC ${sources} )
TARGET_INCLUDE_DIRECTORIES( wtlib-internal 
	PUBLIC
		 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	 )
 ]]
		
	




add_subdirectory(src)


