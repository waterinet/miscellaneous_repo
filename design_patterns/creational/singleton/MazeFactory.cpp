
#include "MazeFactory.h"

using namespace std;

MazeFactory* MazeFactory::_instance = 0;
map<string, MazeFactory*> MazeFactory::_registry;

MazeFactory* MazeFactory::Instance() {
	if (_instance == 0) {
		string name = "EnchantedMazeFactory";

		_instance = Lookup(name);
	}
	return _instance;
}

void MazeFactory::Register(const string& name, MazeFactory* factory) {
    _registry[name] = factory;
}

MazeFactory* MazeFactory::Lookup(const string& name) {
	if (_registry.count(name)) {
		return _registry[name];
	}
	return 0;
}

static BombedMazeFactory bombed_factory;
static EnchantedMazeFactory enchanted_factory;
