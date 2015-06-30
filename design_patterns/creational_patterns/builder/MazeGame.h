#ifndef MAZEGAME_H
#define MAZEGAME_H

#include "MazeBuilder.h"

class MazeGame {
public:
	Maze* CreateMaze(MazeBuilder& builder);
};

#endif // MAZEGAME_H