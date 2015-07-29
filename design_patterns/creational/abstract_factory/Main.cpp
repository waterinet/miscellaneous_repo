#include <iostream>
#include "MazeFactory.h"
#include "MazeGame.h"

using namespace std;

int main() {
	MazeGame game;
	BombedMazeFactory factory;
	
	Maze* m = game.CreateMaze(factory);
	Room* r1 = m->RoomNo(1);
	r1->Enter();
	return 0;
}