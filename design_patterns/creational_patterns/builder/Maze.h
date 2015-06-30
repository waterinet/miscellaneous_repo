#ifndef MAZE_H
#define MAZE_H
#include <map>
#include <iostream>

enum Direction { North, South, East, West };

class MapSite {
public:
	virtual void Enter() = 0;
};

class Room : public MapSite {
public:
	Room(int roomNo) : _roomNumber(roomNo) {}
	
	int GetRoomNo() const {
		return _roomNumber;
	}
	MapSite* GetSide(Direction d) const {
		return _sides[d];
	}
	void SetSide(Direction d, MapSite* s) {
		_sides[d] = s;
	}
	
	virtual void Enter() {
		std::cout << "Enter Room" << std::endl;
	}
	
private:
	MapSite* _sides[4];
	int _roomNumber;
};

class Wall : public MapSite {
public:
	Wall() {}
	
	virtual void Enter() {}
};

class Door : public MapSite {
public:
	Door(Room* r1 = 0, Room* r2 = 0) : 
		_room1(r1), _room2(r2), _isOpen(false) {}
	
	virtual void Enter() {
		std::cout << "Enter Door" << std::endl;
	}
	Room* OtherSideFrom(Room* r) { return 0; }
	
private:
	Room* _room1;
	Room* _room2;
	bool _isOpen;
};

class CommonWall : public Wall {
public:
	CommonWall(Room* r1, Room* r2) : 
		_room1(r1), _room2(r2) {}
	
	virtual void Enter() {}
private:
	Room* _room1;
	Room* _room2;
};

class Maze {
public:
	Maze();
	
	void AddRoom(Room*);
	Room* RoomNo(int) const;

private:
	std::map<int, Room*> _rooms;
};

#endif // MAZE_H