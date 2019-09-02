#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "marker.hpp"
#include "ship.hpp"
#include <vector>

namespace Game
{

class Player : public sf::Drawable
{
	protected:
		std::vector<Ship> m_ships;
		std::unordered_map<sf::Vector2i, Marker> m_markers;
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

		void fillShips() override
		{
			sf::Vector2i start {m_placeGrid->getBounds().left, m_placeGrid->getBounds().top};
			m_ships.push_back(Ship{start.x, start.y + 0, 5});
			m_ships.push_back(Ship{start.x, start.y + 1, 4});
			m_ships.push_back(Ship{start.x, start.y + 2, 3});
			m_ships.push_back(Ship{start.x, start.y + 3, 3});
			m_ships.push_back(Ship{start.x, start.y + 4, 2});
		}

	public:
		HumanPlayer(const BoardGrid* attackGrid, const BoardGrid* defenseGrid, const BoardGrid* placeGrid) :
			Player{attackGrid, defenseGrid}, m_placeGrid{placeGrid}
		{
			fillShips();
		}

		Ship* getShip(sf::Vector2i index)
		{
			for(auto& ship : m_ships)
			{
				if(ship.contains(index))
					return &ship;
			}
			return nullptr;
		}

};

}


#endif