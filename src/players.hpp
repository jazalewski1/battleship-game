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
		virtual void fillShips() = 0;


	public:
		Player(const BoardGrid* attackGrid, const BoardGrid* defenseGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}
		{
		}

		bool isShip(sf::Vector2i index)
		{
			index = index;
			return true;
		}
};


class HumanPlayer : public Player
{
	private:
		const BoardGrid* m_placeGrid;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& ship : m_ships)
				target.draw(ship, states);
			for(const auto& marker : m_markers)
				target.draw(marker.second);
		}

		void fillShips()
		{
			sf::Vector2i start {dynamic_cast<const BoardGrid*>(m_placeGrid)->getBounds()};
			m_ships.push_back(ShipGrid{start.x, start.y + 0, 5});
			m_ships.push_back(ShipGrid{start.x, start.y + 1, 4});
			m_ships.push_back(ShipGrid{start.x, start.y + 2, 3});
			m_ships.push_back(ShipGrid{start.x, start.y + 3, 3});
			m_ships.push_back(ShipGrid{start.x, start.y + 4, 2});
		}

	public:
		HumanPlayer(const BoardGrid* attackGrid, const BoardGrid* defenseGrid, const BoardGrid* placeGrid) :
			Player{attackGrid, defenseGrid}, m_placeGrid{placeGrid}
		{
		}



};

}


#endif