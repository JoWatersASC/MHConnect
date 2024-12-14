set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES src/client.cpp src/server.cpp src/chat_room.cpp) 
set(HEADER_FILES "${INCLUDE_DIR}/chat_room.h" "${INCLUDE_DIR}/server.h" "${INCLUDE_DIR}/client.h")

set(EX_NAME "MHConnectServer")
add_executable(${EX_NAME} main.cpp ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(${EX_NAME} PROPERTIES OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

target_include_directories(${EX_NAME} PRIVATE ${INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/common")
target_link_libraries(${EX_NAME} INTERFACE commonlib)