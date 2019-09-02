#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "marker.hpp"
#include <vector>

namespace Game
{

class Player : public sf::Drawable
{
	protected:
		std::unordered_map<sf::Vector2i, Marker> m_markers;
		std::vector<ShipGrid> m_ships;
		const BoardGrid* m_attackGrid;
		const BoardGrid* m_defenseGrid;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;


	public:
		Player(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}
		{
		}

		bool isShip(sf::Vector2i index)
		{
		}
};


class HumanPlayer : public Player
{
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& ship : m_ships)
				target.draw(ship, states);
			for(const auto& marker : m_markers)
				target.draw(marker.second);
		}

	public:
		HumanPlayer(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			Player{attackGrid, defenseGrid}
		{
		}



};

}


#endif