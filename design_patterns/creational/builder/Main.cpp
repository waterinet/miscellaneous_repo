#include <iostream>
#include "MazeGame.h"
#include "MazeBuilder.h"

using namespace std;

int main() {
	Maze* maze;
	MazeGame game;
	StandardMazeBuilder builder;

	game.CreateMaze(builder);
	maze = builder.GetMaze();
	Room* room = maze->RoomNo(1);
	room->Enter();
	return 0;
}