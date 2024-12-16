# Client side application

set(CMAKE_PREFIX_PATH "C:/sdk/lib/rtaudio")

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES src/client.cpp src/audio.cpp)
set(HEADER_FILES "${INCLUDE_DIR}/client.h" "${INCLUDE_DIR}/audio.h" "${INCLUDE_DIR}/core.h")

set(LIB_NAME "MHConnectClientCore")
add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common")

if(UNIX)
	find_package(PkgConfig REQUIRED)
    pkg_check_modules(RTAUDIO REQUIRED rtaudio)
    target_link_libraries(${LIB_NAME} PUBLIC ${RTAUDIO_LIBRARIES})
    target_include_directories(${LIB_NAME} PUBLIC ${RTAUDIO_INCLUDE_DIRS})
endif()

target_link_libraries(${LIB_NAME} INTERFACE commonlib)

if(WIN32)
	
	option(DOWNLOAD_RTAUDIO "Would you like to download RT Audio? (ON for Yes, OFF for No)" OFF)
	
	message(STATUS "DOWNLOAD_RTAUDIO is set to ${DOWNLOAD_RTAUDIO}")
	
	if(DOWNLOAD_RTAUDIO)
		include(ExternalProject)

		ExternalProject_Add(
			rt_audio
			GIT_REPOSITORY https://github.com/thestk/rtaudio.git
			GIT_TAG        master
			PREFIX         ${CMAKE_BINARY_DIR}/externals/MyLibrary
			INSTALL_DIR    ${CMAKE_BINARY_DIR}/install
			CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
			BUILD_COMMAND   ${CMAKE_COMMAND} --build <BINARY_DIR>
			INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
		)


		add_dependencies(${LIB_NAME} rt_audio)
		
		target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_BINARY_DIR}/install/include)
		target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_BINARY_DIR}/install/lib/rtaudio/Release)
		
		# COMMAND cmake -B build -S .
	else()
		set(RTAUDIO_PATH "C:/sdk/lib/rtaudio/Release")
		set(RTAUDIO_INSTALLATION_PATH "C:/sdk/repos/rtaudio")
		
		target_include_directories(${LIB_NAME} PUBLIC ${RTAUDIO_INSTALLATION_PATH})
		target_link_directories(${LIB_NAME} PUBLIC ${RTAUDIO_PATH})
		target_link_libraries(${LIB_NAME} PUBLIC rtaudio)
	endif()
	
endif()

set(EX_NAME "MHConnectClient")
add_executable(${EX_NAME} main.cpp)
target_link_libraries(${EX_NAME} PRIVATE ${LIB_NAME})