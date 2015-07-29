#include <iostream>
#include "MazeFactory.h"
#include "MazeGame.h"

using namespace std;

int main() {
	MazeGame game;
	
	Maze* m = game.CreateMaze();
	Room* r1 = m->RoomNo(1);
	r1->Enter();
	return 0;
}
