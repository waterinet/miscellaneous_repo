project(factory_method)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("../common")

set(SRC_LIST
  Main.cpp
  ../common/Maze.cpp
  MazeGame.cpp)

add_executable(factory_method ${SRC_LIST})
