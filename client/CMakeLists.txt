# Client side application

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES src/client.cpp)
set(HEADER_FILES "${INCLUDE_DIR}/client.h")

# find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)

set(EX_NAME "MHConnect")
add_executable(${EX_NAME} main.cpp ${SOURCE_FILES})
# set_target_properties(PROPERTIES OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

target_include_directories(${EX_NAME} PRIVATE ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common")

# target_link_libraries(${EX_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

target_link_libraries(${EX_NAME} ${RTAUDIO_LIBRARIES})
target_include_directories(${EX_NAME} PRIVATE ${RTAUDIO_INCLUDE_DIRS})

# find_package(OpenCV REQUIRED)
# target_link_libraries(${EX_NAME} PRIVATE ${OpenCV_LIBS})