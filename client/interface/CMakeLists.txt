add_executable(${HEADLESS_EX} main.cpp)
target_link_libraries(${HEADLESS_EX} PRIVATE ${LIB_NAME})

set(GUI_EX "MHConnect")

set(RTAUDIO_PATH "C:/sdk/lib/rtaudio/Release")
set(RTAUDIO_INSTALLATION_PATH "C:/sdk/repos/rtaudio")

target_include_directories(${HEADLESS_EX} PRIVATE ${RTAUDIO_INSTALLATION_PATH})
target_link_directories(${HEADLESS_EX} PRIVATE ${RTAUDIO_PATH})
target_link_libraries(${HEADLESS_EX} PRIVATE rtaudio)