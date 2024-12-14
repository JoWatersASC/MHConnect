cmake_minimum_required(VERSION 3.15)

project(MHConnect)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if(WIN32)
 #   if(MSVC)  # Check if using the Microsoft compiler
  #      link_libraries(ws2_32)
   # elseif(MINGW)  # For MinGW or other compilers
    #    link_libraries(ws2_32)
   # endif()
#endif()

add_subdirectory(common)

add_subdirectory(client)
add_subdirectory(server)
