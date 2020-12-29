#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Grid.hpp"
#include "game/Marker.hpp"
#include "util/Random.hpp"
#include "game/Ship.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>




namespace game
{
using random_static = effolkronium::random_static;

class Player : public sf::Drawable
{

	protected:
		std::vector<Ship> m_ships;

		std::unordered_map<sf::Vector2i, Marker> m_markers;
		std::unordered_set<sf::Vector2i> m_possibleShots;

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
			sf::IntRect bounds {m_attackGrid->get_bounds()};
			for(int y = 0; y < bounds.height; ++y)
			{
				for(int x = 0; x < bounds.width; ++x)
				{
					m_possibleShots.emplace(x + bounds.left, y + bounds.top);
				}
			}
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
		bool shootable(sf::Vector2i index) const { return m_possibleShots.find(index) != m_possibleShots.end(); }
		bool shootable(sf::Vector2f pos) const { return shootable(common::screen_position_to_index(pos)); }

		virtual void markShot(sf::Vector2i index, bool isHit)
		{
			Marker::Type type {(isHit ? Marker::Type::HIT : Marker::Type::MISS)};
			auto search {m_markers.find(index)};
			if(search != m_markers.end())
				search->second.set_type(type);
			else
				m_markers.emplace(index, Marker{index, type});

			if(m_possibleShots.find(index) != m_possibleShots.end())
				m_possibleShots.erase(index);

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
}

#endif