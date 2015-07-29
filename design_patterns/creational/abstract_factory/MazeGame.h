#ifndef MAZEGAME_H
#define MAZEGAME_H

#include "MazeFactory.h"

class MazeGame {
public:
	Maze* CreateMaze(MazeFactory& factory);
};

#endif // MAZEGAME_H