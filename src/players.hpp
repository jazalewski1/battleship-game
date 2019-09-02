#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include <vector>

namespace Game
{

class Player : public sf::Drawable
{
	private:
		// hit markers
		// miss markers
		// dont put the draw function here at all

		std::vector<ShipGrid> m_ships;
		const BoardGrid* m_attackGrid;
		const BoardGrid* m_defenseGrid;


	public:
		Player(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}
		{

		}

		bool checkShot(sf::Vector2i index)
		{
		}
};

}


#endif