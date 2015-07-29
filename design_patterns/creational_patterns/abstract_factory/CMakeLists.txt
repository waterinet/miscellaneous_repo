project(abstract_factory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("../common")

set(SRC_LIST
  Main.cpp
  ../common/Maze.cpp
  MazeFactory.cpp
  MazeGame.cpp)

add_executable(abstract_factory ${SRC_LIST})
