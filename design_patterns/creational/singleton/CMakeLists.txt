project(singleton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

include_directories("../common")

set(SRC_LIST
  Main.cpp
  ../common/Maze.cpp
  MazeFactory.cpp
  MazeGame.cpp)

add_executable(singleton ${SRC_LIST})
