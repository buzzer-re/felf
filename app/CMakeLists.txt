# add_executable(quickelf main.cpp)
include(GNUInstallDirs)
add_library(felf++ SHARED ELF.cpp)
file(GLOB HEADERS ELF.h)

install(TARGETS felf++
    LIBRARY DESTINATION /lib
    PUBLIC_HEADER DESTINATION ${INCLUDE})


install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})