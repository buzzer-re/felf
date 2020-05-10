add_executable(quickelf main.cpp)
add_library(ELF ELF.cpp)
target_link_libraries(quickelf ELF)
