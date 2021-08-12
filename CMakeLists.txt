cmake_minimum_required(VERSION 3.0)
project(world)
aux_source_directory(engine DIR_SRCS)
aux_source_directory(thirdparty/glad DIR_SRCS)
message("source files: ${DIR_SRCS}")
link_directories(world thirdparty/glfw/build/src)
add_executable(world ${DIR_SRCS})

target_link_libraries(world GLEW glfw3 GL X11 pthread Xrandr Xi dl)