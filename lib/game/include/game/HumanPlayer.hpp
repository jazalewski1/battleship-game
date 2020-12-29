#pragma once

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Grid.hpp"
#include "game/Marker.hpp"
#include "game/Player.hpp"
#include "game/Ship.hpp"
#include "util/Random.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace game
{
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
			sf::Vector2i start {m_placeGrid->get_bounds().left, m_placeGrid->get_bounds().top};
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
			sf::IntRect gridBounds {m_defenseGrid->get_bounds()};
			for(const auto& ship : m_ships)
			{
				sf::IntRect shipBounds {ship.getBounds()};
				if(!(gridBounds.contains(shipBounds.left, shipBounds.top) &&
					gridBounds.contains(shipBounds.left + shipBounds.width - 1, shipBounds.top + shipBounds.height - 1)))
					return false;
			}
			return true;
		}

		void markGuess(sf::Vector2i index)
		{
			if(m_attackGrid->contains(index))
			{
				auto search {m_markers.find(index)};
				if(search != m_markers.end())
				{
					if(search->second.is_guess())
						m_markers.erase(search);
				}
				else
					m_markers.emplace(index, Marker{index, Marker::Type::GUESS});
			}
		}
};
}