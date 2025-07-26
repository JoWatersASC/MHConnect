add_library(commonlib INTERFACE)
target_include_directories(commonlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_options(commonlib PRIVATE "-pthread")

if(WIN32)
	target_link_libraries(commonlib INTERFACE ws2_32)
endif()