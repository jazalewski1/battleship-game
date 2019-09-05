#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "marker.hpp"
#include "random.hpp"
#include "ship.hpp"
#include <iostream>
#include <vector>


using random = effolkronium::random_static;


namespace Game
{
class Player : public sf::Drawable
{
	protected:
		std::vector<Ship> m_ships;
		std::unordered_map<sf::Vector2i, Marker> m_markers;
		const Grid* m_attackGrid;
		const Grid* m_defenseGrid;
		const Grid* m_placeGrid;
		int m_points;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
		virtual void fillShips() = 0;


	public:
		Player(const Grid* attackGrid, const Grid* defenseGrid, const Grid* placeGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}, m_placeGrid{placeGrid},
			m_points{0}
		{
		}

		bool isShip(sf::Vector2i index) const
		{
			for(const auto& ship : m_ships)
			{
				if(ship.contains(index))
					return true;
			}
			return false;
		}

		bool placeable(const Ship* test) const
		{
			sf::IntRect rect {test->getGhostBounds()};
			for(int y = 0; y < rect.height; ++y)
			{
				for(int x = 0; x < rect.width; ++x)
				{
					sf::Vector2i index {rect.left + x, rect.top + y};
					if(!m_defenseGrid->contains(index))
						return false;
				}
			}
			for(int y = 0; y < rect.height + 2; ++y)
			{
				for(int x = 0; x < rect.width + 2; ++x)
				{
					sf::Vector2i index {rect.left - 1 + x, rect.top - 1 + y};
					if(test == getShip(index))
						continue;

					if(isShip(index))
						return false;
				}
			}
			return true;
		}
		bool shootable(sf::Vector2i index) const { return m_markers.find(index) == m_markers.end(); }
		bool shootable(sf::Vector2f pos) const { return m_markers.find(ftoi(pos)) == m_markers.end(); }
		void markShot(sf::Vector2i index, bool isHit)
		{
			m_markers.emplace(index, Marker{index, isHit});

			if(isHit)
				++m_points;
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
		const Ship* getShip(sf::Vector2i index) const
		{
			for(auto& ship : m_ships)
			{
				if(ship.contains(index))
					return &ship;
			}
			return nullptr;
		}
		int getPoints() const { return m_points; }

};


class HumanPlayer : public Player
{
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			for(const auto& ship : m_ships)
				target.draw(ship, states);
			for(const auto& marker : m_markers)
				target.draw(marker.second, states);
		}

		void fillShips() override
		{
			sf::Vector2i start {m_placeGrid->getBounds().left, m_placeGrid->getBounds().top};
			m_ships.push_back(Ship{start.x, start.y + 0, 5});
			m_ships.push_back(Ship{start.x, start.y + 2, 4});
			m_ships.push_back(Ship{start.x, start.y + 4, 3});
			m_ships.push_back(Ship{start.x, start.y + 6, 3});
			m_ships.push_back(Ship{start.x, start.y + 8, 2});
		}

	public:
		HumanPlayer(const Grid* attackGrid, const Grid* defenseGrid, const Grid* placeGrid) :
			Player{attackGrid, defenseGrid, placeGrid}
		{
			fillShips();
		}

		bool isReady() const
		{
			sf::IntRect gridBounds {m_defenseGrid->getBounds()};
			for(const auto& ship : m_ships)
			{
				sf::IntRect shipBounds {ship.getBounds()};
				if(!(gridBounds.contains(shipBounds.left, shipBounds.top) &&
					gridBounds.contains(shipBounds.left + shipBounds.width - 1, shipBounds.top + shipBounds.height - 1)))
					return false;
			}
			return true;
		}
};


class ComputerPlayer : public Player
{
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// for(const auto& ship : m_ships)
			// 	target.draw(ship, states);
			for(const auto& marker : m_markers)
				target.draw(marker.second, states);
		}


		void fillShips() override
		{
			sf::Vector2i start {m_placeGrid->getBounds().left, m_placeGrid->getBounds().top};
			sf::IntRect bounds {m_defenseGrid->getBounds()};
			int shipSize;
			for(int i = 1; i <= 5; ++i)
			{
				shipSize = (i < 3 ? i + 1 : i);

				sf::Vector2i temp;
				m_ships.push_back(Ship{start, shipSize});
				do
				{
					if(random::get(0, 100) < 50)
						m_ships.back().rotate();
					sf::Vector2i offset {random::get(bounds.left, bounds.left + bounds.width), random::get(bounds.top, bounds.top + bounds.height)};
					temp = offset;
					m_ships.back().setOffset(offset);
				} while(!placeable(&m_ships.back()));
			}
		}

	public:
		ComputerPlayer(const Grid* attackGrid, const Grid* defenseGrid, const Grid* placeGrid) :
			Player{attackGrid, defenseGrid, placeGrid}
		{
			fillShips();
		}

		sf::Vector2i makeShot() const
		{
			// brain, deciding where the shot should be, based on markers, probable indexes etc.
			// keep track of destroyed sizes, for that also count consecutive hit shots
			// keep track of last shot, and direction (int unit vector)
			// if last shot was hit, set some direction and follow it or not

			sf::IntRect bounds {m_attackGrid->getBounds()};
			sf::Vector2i index;
			do
			{
				index.x = random::get(0, bounds.width - 1);
				index.y = random::get(0, bounds.height - 1);
				index += sf::Vector2i{bounds.left, bounds.top};
			} while(!shootable(index));

			return index;
		}
};



}


#endif