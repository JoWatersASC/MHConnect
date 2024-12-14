add_library(commonlib INTERFACE common.h MHwindows.h threadpool.h)
if(WIN32)
	target_link_libraries(commonlib INTERFACE ws2_32)
endif()