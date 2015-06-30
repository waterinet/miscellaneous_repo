#include <iostream>
#include "MazeGame.h"

using namespace std;

int main() {
    MazeGame* game = new BombedMazeGame();
    Maze* maze = game->CreateMaze();
    Room* r = maze->RoomNo(1);
	r->Enter();

    return 0;
}
