#ifndef MAZEGAME_H
#define MAZEGAME_H

#include "Maze.h"

class MazeGame {
public:
	Maze* CreateMaze();

	//factory methods

	virtual Maze* MakeMaze() const {
		return new Maze; 
	}
	virtual Room* MakeRoom(int n) const {
		return new Room(n); 
	}
	virtual Wall* MakeWall() const {
		return new Wall; 
	}
	virtual Door* MakeDoor(Room* r1, Room* r2) const {
		return new Door(r1, r2); 
	}
};

class BombedMazeGame : public MazeGame {
public:
	BombedMazeGame() {}

	virtual Wall* MakeWall() const {
		return new BombedWall;
	}

	virtual Room* MakeRoom(int n) const {
		return new RoomWithABomb(n);
	}
};

#endif // MAZEGAME_H
