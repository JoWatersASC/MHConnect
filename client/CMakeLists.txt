set(LIB_NAME "MHConnectClientCore")
set(H_EX_NAME "MHConnectClient_Headless")
set(EX_NAME "MHConnectClient")


option(BUILD_SHARED_LIBS OFF)
option(HEADLESS_BUILD ON)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES)
set(HEADER_FILES)



if(HEADLESS_BUILD)
message(STATUS "HEADLESS BUILD BUILT")

set(SOURCE_FILES
    src/client_h.cpp
)
set(HEADER_FILES
    "${INCLUDE_DIR}/client_h.h"
    "${INCLUDE_DIR}/core.h"
)

if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES}) 
else()
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
endif() # BUILD_SHARED_LIBS

add_executable(${EX_NAME} main_h.cpp)
# return()
else() # HEADLESS_BUILD
message(STATUS "GUI BUILD BUILT")

set(SOURCE_FILES
    src/client.cpp
    src/audio.cpp
)
set(HEADER_FILES
    "${INCLUDE_DIR}/client.h"
    "${INCLUDE_DIR}/audio.h"
    "${INCLUDE_DIR}/core.h"
)

include(FetchContent)
#-------------------------
# Fetch RtAudio Content
#-------------------------
find_package(RtAudio)
if(RtAudio_FOUND)
	message(STATUS "RtAudio found in system")
	
	if(NOT TARGET rtaudio)
        add_library(rtaudio INTERFACE IMPORTED)
        target_include_directories(rtaudio INTERFACE ${RtAudio_INCLUDE_DIRS})
        target_link_libraries(rtaudio INTERFACE ${RtAudio_LIBRARIES})
        message(STATUS "Created wrapper target for legacy RtAudio variables.")
    endif()
else()
	message(STATUS "RtAudio NOT found in system, fetching content")
	FetchContent_Declare(
		RTAudio
		GIT_REPOSITORY https://github.com/thestk/rtaudio.git
		GIT_TAG        6.0.0
	) 
	FetchContent_MakeAvailable(RTAudio)
endif()# RtAudio_FOUND

if (TARGET rtaudio)
    message(STATUS "RtAudio found/fetched and target 'rtaudio' is available.")
else()
    message(FATAL_ERROR "RtAudio target 'rtaudio' not found after find/fetch. Check RtAudio's CMake configuration.")
endif()

#-------------------------
# Fetch OpenCV Content
#-------------------------
set(OPENCV_REQUIRED_COMPONENTS imgproc videoio)

find_package(OpenCV CONFIG QUIET COMPONENTS ${OPENCV_REQUIRED_COMPONENTS})

if(NOT OpenCV_FOUND)
	FetchContent_Declare(
    	OpenCV
    	GIT_REPOSITORY https://github.com/opencv/opencv.git
    	GIT_TAG        4.9.0
	)
	FetchContent_MakeAvailable(OpenCV)
	
	set(OPENCV_LIB_TARGETS
		opencv_imgproc
		opencv_videoio
	)

else()
	set(OPENCV_LIB_TARGETS ${OpenCV_LIBS})
endif()

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)

#[[
if(NOT TARGET OpenCV::videoio)
	add_library(OpenCV::all_components INTERFACE IMPORTED)
	target_include_directories(OpenCV::all_components INTERFACE ${OpenCV_INCLUDE_DIRS})
	target_link_libraries(OpenCV::all_components INTERFACE ${OpenCV_LIBRARIES})

	set(OPENCV_LINK_TARGET OpenCV::all_components)
else()
	set(OPENCV_LINK_TARGET OpenCV::imgproc OpenCV::videoio)
endif()
]]
set(OPENCV_TARGETS "")
foreach(comp ${OPENCV_REQUIRED_COMPONENTS})
    list(APPEND OPENCV_TARGETS opencv_${comp})
endforeach()

if(OpenCV_FOUND)
    message(STATUS "Found OpenCV: ${OpenCV_VERSION}")
endif()

#-------------------------
# Fetch wxWidgets Content
#-------------------------
set(wxWidgets_COMPONENTS core base)
find_package(wxWidgets QUIET COMPONENTS ${wxWidgets_COMPONENTS})

if(wxWidgets_FOUND)
    message(STATUS "Found system installed wxWidgets: ${wxWidgets_VERSION}")
	
	if(NOT TARGET wx::wxWidgets)
        add_library(wx::wxWidgets INTERFACE IMPORTED)
        target_include_directories(wx::wxWidgets INTERFACE ${wxWidgets_INCLUDE_DIRS})
        target_link_libraries(wx::wxWidgets INTERFACE ${wxWidgets_LIBRARIES})
        target_compile_definitions(wx::wxWidgets INTERFACE ${wxWidgets_DEFINITIONS})
        if(wxWidgets_CXX_FLAGS)
            target_compile_options(wx::wxWidgets INTERFACE ${wxWidgets_CXX_FLAGS})
        endif()
        message(STATUS "Created wrapper target for system wxWidgets.")
    endif()
else()
    message(STATUS "System installed wxWidgets NOT found, fetching content")
    
    FetchContent_Declare(
        wxwidgets_proj
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG        v3.2.5
    )
    set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(wxBUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(wxUSE_GUI ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(wxwidgets_proj)

	if(NOT TARGET wx::wxWidgets)
		add_library(wx::wxWidgets INTERFACE IMPORTED)
		target_link_libraries(wx::wxWidgets INTERFACE wx::core wx::base)
		message(STATUS "Created wrapper target for legacy wxWidgets find module.")
	endif()
endif() # wxWidgets_FOUND


if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
else()
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
endif() # BUILD_SHARED_LIBS

add_executable(${EX_NAME} main.cpp
    src/main_gui.cpp src/start_gui.cpp src/video.cpp
	"${INCLUDE_DIR}/gui.h" "${INCLUDE_DIR}/video.h")

target_link_libraries(${LIB_NAME} PUBLIC rtaudio)
target_link_libraries(${EX_NAME} PRIVATE ${OPENCV_LIB_TARGETS} wx::wxWidgets)
target_include_directories(${EX_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

get_target_property(WXWIDGETS_INCLUDES wx::wxWidgets INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "DEBUG: wxWidgets include directories are: ${WXWIDGETS_INCLUDES}")
endif() # HEADLESS_BUILD

set_target_properties(${LIB_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
)

target_include_directories(${LIB_NAME} PUBLIC
    ${INCLUDE_DIR}
    "${CMAKE_SOURCE_DIR}/common"
)

target_link_libraries(${LIB_NAME} INTERFACE commonlib)
target_link_libraries(${EX_NAME} PRIVATE ${LIB_NAME})
