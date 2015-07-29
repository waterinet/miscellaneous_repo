project(builder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("../common")

set(SRC_LIST
  Main.cpp
  ../common/Maze.cpp
  MazeBuilder.cpp
  MazeGame.cpp)

add_executable(builder ${SRC_LIST})
