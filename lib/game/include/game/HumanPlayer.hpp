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
			for(const auto& ship : ships)
				target.draw(ship, states);
			for(const auto& marker : markers)
				target.draw(marker.second, states);
		}

		void fill_ships() override
		{
			sf::Vector2i start {place_grid.get_bounds().left, place_grid.get_bounds().top};
			ships.push_back(Ship{start.x, start.y + 0, 5});
			ships.push_back(Ship{start.x, start.y + 2, 4});
			ships.push_back(Ship{start.x, start.y + 4, 3});
			ships.push_back(Ship{start.x, start.y + 6, 3});
			ships.push_back(Ship{start.x, start.y + 8, 2});
		}

	public:
		HumanPlayer(Grid& attack_grid, Grid& defense_grid, Grid& place_grid) :
			Player{attack_grid, defense_grid, place_grid}
		{
			fill_ships();
		}

		bool isReady() const
		{
			sf::IntRect gridBounds {defense_grid.get_bounds()};
			for(const auto& ship : ships)
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
			if(attack_grid.contains(index))
			{
				auto search {markers.find(index)};
				if(search != markers.end())
				{
					if(search->second.is_guess())
						markers.erase(search);
				}
				else
					markers.emplace(index, Marker{index, Marker::Type::GUESS});
			}
		}
};
}