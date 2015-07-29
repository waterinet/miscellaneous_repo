#ifndef MAZEFACTORY_H
#define MAZEFACTORY_H

#include <map>
#include <string>
#include "Maze.h"

class MazeFactory {
public:
	static MazeFactory* Instance();
    static void Register(const std::string& name, MazeFactory*);

    virtual Maze* MakeMaze() const {
        return new Maze;
    }
    virtual Wall* MakeWall() const {
        return new Wall;
    }
    virtual Room* MakeRoom(int n) const {
        return new Room(n);
    }
    virtual Door* MakeDoor(Room* r1, Room* r2) const {
        return new Door(r1, r2);
    }
protected:
	MazeFactory() {}
    static MazeFactory* Lookup(const std::string&);
private:
	static MazeFactory* _instance;
	static std::map<std::string, MazeFactory*> _registry;
};

class BombedMazeFactory : public MazeFactory {
public:
    BombedMazeFactory() {
		MazeFactory::Register("BombedMazeFactory", this);
	}

    Wall* MakeWall() const {
        return new BombedWall;
    }

    Room* MakeRoom(int n) const {
        return new RoomWithABomb(n);
    }
};

class EnchantedMazeFactory : public MazeFactory {
public:
	EnchantedMazeFactory() {
		MazeFactory::Register("EnchantedMazeFactory", this);
	}

	virtual Room* MakeRoom(int n) const {
		return new EnchantedRoom(n, CastSpell());
	}

	virtual Door* MakeDoor(Room* r1, Room* r2) const {
        return new DoorNeedingSpell(r1, r2);
	}
protected:
	Spell* CastSpell() const {
		return new Spell;
	}
};

#endif
