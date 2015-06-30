#include "Maze.h"

Maze::Maze() {
}

void Maze::AddRoom(Room* r) {
	_rooms[r->GetRoomNo()] = r;
}

Room* Maze::RoomNo(int n) const {
	auto it = _rooms.find(n);
	return (it != _rooms.end()) ? it->second : 0;
}