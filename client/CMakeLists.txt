# Client side application
# set(CMAKE_PREFIX_PATH "C:/sdk/lib/rtaudio")

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES src/client.cpp src/audio.cpp)
set(HEADER_FILES "${INCLUDE_DIR}/client.h" "${INCLUDE_DIR}/audio.h")

# find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

if(Win32)
    set(RTAUDIO_PATH "path/to/rtaudio")
    set(RTAUDIO_INSTALLATION_PATH "path/to/rtaudio/clone")
endif()

if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RTAUDIO REQUIRED rtaudio)
endif()


set(EX_NAME "MHConnect")
add_executable(${EX_NAME} main.cpp ${SOURCE_FILES} ${HEADER_FILES})
# set_target_properties(PROPERTIES OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

target_include_directories(${EX_NAME} PRIVATE ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common")

# target_link_libraries(${EX_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

if(WIN32)
    target_include_directories(${EX_NAME} PRIVATE ${RTAUDIO_INSTALLATION_PATH})
    target_link_directories(${EX_NAME} PRIVATE ${RTAUDIO_PATH}/lib)
    target_link_libraries(${EX_NAME} PRIVATE rtaudio)
endif()

if(UNIX)
    target_link_libraries(${EX_NAME} ${RTAUDIO_LIBRARIES})
    target_include_directories(${EX_NAME} PRIVATE ${RTAUDIO_INCLUDE_DIRS})
endif()
# find_package(OpenCV REQUIRED)
# target_link_libraries(${EX_NAME} PRIVATE ${OpenCV_LIBS})