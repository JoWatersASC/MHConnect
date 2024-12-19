set(LIB_NAME "MHConnectClientCore")
set(EX_NAME "MHConnectClient")

# Client side application

set(CMAKE_PREFIX_PATH "/path/to/rtaudio")

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES src/client.cpp src/audio.cpp src/gui.cpp)
set(HEADER_FILES "${INCLUDE_DIR}/client.h" "${INCLUDE_DIR}/audio.h" "${INCLUDE_DIR}/core.h" "${INCLUDE_DIR}/gui.h")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS ON)

if(BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
else()
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
endif()

set_target_properties(${LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)


target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common" INTERFACE ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common")

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

        else()
                set(RTAUDIO_PATH "path/to/rtaudio/Release")
                set(RTAUDIO_INSTALLATION_PATH "path/to/local/rtaudio/repository")

                target_include_directories(${LIB_NAME} PUBLIC ${RTAUDIO_INSTALLATION_PATH})
                target_link_directories(${LIB_NAME} PUBLIC ${RTAUDIO_PATH})
                target_link_libraries(${LIB_NAME} PUBLIC rtaudio INTERFACE rtaudio)
        endif()
		
		
		#FOR WXWIDGETS
		# target_link_directories(${LIB_NAME} PUBLIC )
		add_definitions(-D_UNICODE -DUNICODE)
		set(wxWidgets_USE_STATIC OFF)
	
		if(wxWidgets_USE_STATIC)
			target_link_directories(${LIB_NAME} PUBLIC "$ENV{WXWIN}/lib/vc_x64_lib")
		else()
			add_definitions(-DWXUSINGDLL)
			target_link_directories(${LIB_NAME} PUBLIC "$ENV{WXWIN}/lib/vc_x64_dll")
		endif()
		
		target_include_directories(${LIB_NAME} PUBLIC "$ENV{WXWIN}/include" "$ENV{WXWIN}/include/msvc")
		
		set(WX_DLIBS wxbase32ud wxbase32ud_net wxmsw32ud_core wxbase32ud_xml wxmsw32ud_adv wxmsw32ud_aui wxmsw32ud_media wxmsw32ud_gl wxmsw32ud_qa wxmsw32ud_ribbon wxmsw32ud_richtext wxmsw32ud_stc)
		set(WX_RLIBS wxbase32u wxbase32u_net wxmsw32u_core wxbase32u_xml wxmsw32u_adv wxmsw32u_aui wxmsw32u_media wxmsw32u_gl wxmsw32u_qa wxmsw32u_ribbon wxmsw32u_richtext wxmsw32u_stc)
		
		target_link_libraries(${LIB_NAME} PUBLIC ${WX_DLIBS} INTERFACE ${WX_DLIBS})

elseif(UNIX)
    find_package(wxWidgets REQUIRED COMPONENTS core base)
	include(${wxWidgets_USE_FILE})
	execute_process(COMMAND wx-config --cxxflags OUTPUT_VARIABLE WX_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND wx-config --libs OUTPUT_VARIABLE WX_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		
    target_link_libraries(${LIB_NAME} PRIVATE ${wxWidgets_LIBRARIES})
    target_include_directories(${LIB_NAME} PUBLIC ${wxWidgets_INCLUDE_DIRS}) 
	target_compile_options(${LIB_NAME} PUBLIC ${WX_CXXFLAGS})
	target_link_libraries(${LIB_NAME} PUBLIC ${WX_LIBS})

	pkg_check_modules(GTK REQUIRED gtk+-3.0)
	target_include_directories(${LIB_NAME} PUBLIC ${GTK3_INCLUDE_DIRS})
    target_link_libraries(${LIB_NAME} PUBLIC ${GTK3_LIBRARIES})
	target_compile_options(${LIB_NAME} PUBLIC ${GTK_CFLAGS_OTHER})
endif()

add_executable(${EX_NAME} main.cpp)
target_link_libraries(${EX_NAME} PRIVATE ${LIB_NAME})

set_target_properties(${EX_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(BUILD_SHARED_LIBS)
	add_custom_command(TARGET ${EX_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIB_NAME}> ${CMAKE_BINARY_DIR}/bin/Debug)
	add_custom_command(TARGET ${EX_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIB_NAME}> ${CMAKE_BINARY_DIR}/bin/Release)
endif()

if(UNIX)
endif()
