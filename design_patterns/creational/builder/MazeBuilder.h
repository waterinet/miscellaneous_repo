#ifndef MAZEBUILDER_H
#define MAZEBUILDER_H

#include "Maze.h"

class MazeBuilder {
public:
	virtual void BuildMaze() {}
	virtual void BuildRoom(int room) {}
	virtual void BuildDoor(int roomFrom, int roomTo) {}

	virtual Maze* GetMaze() { return 0; }
protected:
	MazeBuilder() {};
};

class StandardMazeBuilder : public MazeBuilder {
public:
	StandardMazeBuilder();

	virtual void BuildMaze();
	virtual void BuildRoom(int);
	virtual void BuildDoor(int, int);

	virtual Maze* GetMaze();
private:
	Direction CommonWall(Room*, Room*);
	Maze* _currentMaze;
};

#endif // MAZEBUILDER_H
