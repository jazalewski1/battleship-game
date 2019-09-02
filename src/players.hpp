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
	protected:
		// hit markers
		// miss markers
		// dont put the draw function here at all

		std::vector<ShipGrid> m_ships;
		const BoardGrid* m_attackGrid;
		const BoardGrid* m_defenseGrid;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;


	public:
		Player(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}
		{
		}

		bool checkShot(sf::Vector2i index)
		{
		}
};


class HumanPlayer : public Player
{
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& ship : m_ships)
				target.draw(ship);
			// for(const auto& marker : m_hitMarkers)
			// 	target.draw(marker);
			// for(const auto& marker : m_missMarkers)
			// 	target.draw(marker);
		}

	public:
		HumanPlayer(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			Player{attackGrid, defenseGrid}
		{
		}



};

}


#endif