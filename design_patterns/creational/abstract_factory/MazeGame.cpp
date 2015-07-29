#include "MazeGame.h"

Maze* MazeGame::CreateMaze(MazeFactory& factory) {
	Maze* m = factory.MakeMaze();
	Room* r1 = factory.MakeRoom(1);
	Room* r2 = factory.MakeRoom(2);
	Door* d = factory.MakeDoor(r1, r2);
	
	m->AddRoom(r1);
	m->AddRoom(r2);
	
	r1->SetSide(North, factory.MakeWall());
	r1->SetSide(East, d);
	r1->SetSide(South, factory.MakeWall());
	r1->SetSide(West, factory.MakeWall());
	r2->SetSide(North, factory.MakeWall());
	r2->SetSide(East, factory.MakeWall());
	r2->SetSide(South, factory.MakeWall());
	r2->SetSide(West, d);
	
	return m;
}