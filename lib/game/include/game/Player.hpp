#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Grid.hpp"
#include "game/Marker.hpp"
#include "game/Ship.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace game
{
class Player : public sf::Drawable
{
public:
	Player(Grid& attack_grid, Grid& defense_grid, Grid& place_grid) :
		attack_grid{attack_grid}, defense_grid{defense_grid}, place_grid{place_grid}, points{0}
	{
		const auto bounds = attack_grid.get_bounds();
		for (int y = 0; y < bounds.height; ++y)
		{
			for (int x = 0; x < bounds.width; ++x)
			{
				possible_shots.emplace(x + bounds.left, y + bounds.top);
			}
		}
	}

	bool contains_ship(sf::Vector2i index) const
	{
		for (const auto& ship : ships) // TODO: std::find_if
		{
			if (ship.contains(index))
			{
				return true;
			}
		}
		return false;
	}

	bool placeable(const Ship* ship) const
	{
		const auto ship_bounds = ship->getGhostBounds();
		for (int y = 0; y < ship_bounds.height; ++y)
		{
			for (int x = 0; x < ship_bounds.width; ++x)
			{
				const auto index = sf::Vector2i{ship_bounds.left + x, ship_bounds.top + y};
				if (!defense_grid.contains(index))
				{
					return false;
				}
			}
		}
		for (int y = 0; y < ship_bounds.height + 2; ++y)
		{
			for (int x = 0; x < ship_bounds.width + 2; ++x)
			{
				const auto index = sf::Vector2i{ship_bounds.left - 1 + x, ship_bounds.top - 1 + y};
				if (ship == get_ship(index))
				{
					continue;
				}

				if (contains_ship(index))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool shootable(sf::Vector2i index) const
	{
		return possible_shots.find(index) != possible_shots.end();
	}
	
	bool shootable(sf::Vector2f pos) const
	{
		return shootable(common::screen_position_to_index(pos));
	}

	virtual void mark_shot(sf::Vector2i index, bool is_hit)
	{
		const auto type = (is_hit ? Marker::Type::HIT : Marker::Type::MISS);

		const auto search = markers.find(index);
		if (search != markers.end())
		{
			search->second.set_type(type);
		}
		else
		{
			markers.emplace(index, Marker{index, type});
		}

		if (possible_shots.find(index) != possible_shots.end())
		{
			possible_shots.erase(index);
		}
		if (is_hit)
		{
			++points;
		}
	}

	Ship* get_ship(sf::Vector2i index)
	{
		for (auto& ship : ships) // TODO: find_if
		{
			if (ship.contains(index))
			{
				return &ship;
			}
		}
		return nullptr;
	}

	const Ship* get_ship(sf::Vector2i index) const
	{
		for (auto& ship : ships) // TODO: find_if
		{
			if (ship.contains(index))
			{
				return &ship;
			}
		}
		return nullptr;
	}

	int get_points() const
	{
		return points;
	}

protected:
	std::vector<Ship> ships;
	std::unordered_map<sf::Vector2i, Marker> markers;
	std::unordered_set<sf::Vector2i> possible_shots;
	Grid& attack_grid;
	Grid& defense_grid;
	Grid& place_grid;
	int points;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual void fillShips() = 0;
};
} // namespace game

#endif