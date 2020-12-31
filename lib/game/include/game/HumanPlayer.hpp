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
public:
	HumanPlayer(Grid& attack_grid, Grid& defense_grid, Grid& place_grid) :
		Player{attack_grid, defense_grid, place_grid}
	{
		fill_ships();
	}

	bool is_ready() const
	{
		const auto grid_bounds = defense_grid.get_bounds();
		for (const auto& ship : ships) // TODO: find_if
		{
			const auto ship_bounds = ship.getBounds();
			if (!(grid_bounds.contains(ship_bounds.left, ship_bounds.top) && // TODO: too verbose
				  grid_bounds.contains(ship_bounds.left + ship_bounds.width - 1, ship_bounds.top + ship_bounds.height - 1)))
			{
				return false;
			}
		}
		return true;
	}

	void mark_guess(sf::Vector2i index)
	{
		if (attack_grid.contains(index))
		{
			const auto search = markers.find(index);
			if (search != markers.end() and search->second.is_guess())
			{
				markers.erase(search);
			}
			else
			{
				markers.emplace(index, Marker{index, Marker::Type::GUESS});
			}
		}
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		for (const auto& ship : ships)
		{
			target.draw(ship, states);
		}
		for (const auto& marker : markers)
		{
			target.draw(marker.second, states);
		}
	}

	void fill_ships() override
	{
		const auto start = sf::Vector2i{place_grid.get_bounds().left, place_grid.get_bounds().top};
		ships.push_back(Ship{start.x, start.y + 0, 5});
		ships.push_back(Ship{start.x, start.y + 2, 4});
		ships.push_back(Ship{start.x, start.y + 4, 3});
		ships.push_back(Ship{start.x, start.y + 6, 3});
		ships.push_back(Ship{start.x, start.y + 8, 2});
	}
};
} // namespace game