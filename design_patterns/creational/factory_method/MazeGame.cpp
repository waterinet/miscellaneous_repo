#include "MazeGame.h"

Maze* MazeGame::CreateMaze() {
	Maze* m = MakeMaze();

	Room* r1 = MakeRoom(1);
	Room* r2 = MakeRoom(2);
	Door* theDoor = MakeDoor(r1, r2);

	m->AddRoom(r1);
	m->AddRoom(r2);

	r1->SetSide(North, MakeWall());
	r1->SetSide(East, theDoor);
	r1->SetSide(South, MakeWall());
	r1->SetSide(West, MakeWall());

	r2->SetSide(North, MakeWall());
	r2->SetSide(East, MakeWall());
	r2->SetSide(South, MakeWall());
	r2->SetSide(West, theDoor);

	return m;
}
