#include "Player.h"
#include "GameObject.h"

int main() {
	std::vector<Player> p;
	std::vector<int> v{1, 2};
	GameObject g(0, 1000.0, v, p, 2);
}
