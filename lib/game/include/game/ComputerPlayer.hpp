#pragma once

#include "game/Player.hpp"
#include "util/Random.hpp"

namespace game
{
using random_static = effolkronium::random_static;

class ComputerPlayer : public Player
{
public:
	ComputerPlayer(Grid& attack_grid, Grid& defense_grid, Grid& place_grid) :
		Player{attack_grid, defense_grid, place_grid}, found_origin{false}, found_plane{false}, found_end1{false}, found_end2{false},
		delay_limit{60}, delay_counter{0}
	{
		fill_ships();
		set_direction(static_cast<Dir>(random_static::get(0, 3)));
	}

	sf::Vector2i make_shot()
	{
		if (!found_origin and !found_plane)
		{
			if (last_shot.hit)
			{
				found_origin = true;
				origin = last_shot.index;

				auto index = sf::Vector2i{origin + diff};
				while (!shootable(index))
				{
					rotate_direction();
					index = origin + diff;
				}
				return index;
			}
			else
			{
				const auto random_shot = random_static::get(possible_shots);
				return *random_shot;
			}
		}

		if (found_origin and !found_plane)
		{
			if (!last_shot.hit)
			{
				rotate_direction();

				auto index = sf::Vector2i{origin + diff};

				while (!shootable(index))
				{
					rotate_direction();
					index = origin + diff;
				}
				return index;
			}
			else
			{
				found_plane = true;

				auto index = sf::Vector2i{last_shot.index + diff};
				auto test = true;

				while (test)
				{
					if (!attack_grid.contains(index))
					{
						found_end1 = true;
						reverse_direction();
						test = false;
					}
					else
					{
						const auto marker_itr = markers.find(index);
						if (marker_itr != markers.end())
						{
							if (marker_itr->second.is_hit())
							{
								index = index + diff;
							}
							else
							{
								found_end1 = true;
								reverse_direction();
								test = false;
							}
						}
						else
						{
							return index;
						}
					}
				}
			}
		}
		if (found_origin and found_plane)
		{
			if (!found_end1 and !found_end2)
			{
				if (last_shot.hit)
				{
					auto index = sf::Vector2i{last_shot.index + diff};
					auto test = true;

					while (test)
					{
						if (!attack_grid.contains(index))
						{
							found_end1 = true;
							reverse_direction();
							test = false;
						}
						else
						{
							const auto marker_itr = markers.find(index);

							if (marker_itr != markers.end())
							{
								if (marker_itr->second.is_hit())
								{
									index = index + diff;
								}
								else
								{
									found_end1 = true;
									reverse_direction();
									test = false;
								}
							}
							else
							{
								return index;
							}
						}
					}
				}
				else
				{
					found_end1 = true;
					reverse_direction();

					auto index = sf::Vector2i{last_shot.index + diff};
					auto test = true;

					while (test)
					{
						if (!attack_grid.contains(index))
						{
							found_end2 = true;
							test = false;
						}
						else
						{
							auto marker_itr{markers.find(index)};
							if (marker_itr != markers.end())
							{
								if (marker_itr->second.is_hit())
								{
									index = index + diff;
								}
								else
								{
									found_end2 = true;
									test = false;
								}
							}
							else
							{
								return index;
							}
						}
					}
				}
			}
			if (found_end1 and !found_end2)
			{
				if (last_shot.hit)
				{
					auto index = sf::Vector2i{last_shot.index + diff};
					auto test = true;

					while (test)
					{
						if (!attack_grid.contains(index))
						{
							found_end2 = true;
							test = false;
						}
						else
						{
							auto marker_itr{markers.find(index)};
							if (marker_itr != markers.end())
							{
								if (marker_itr->second.is_hit())
								{
									index = index + diff;
								}
								else
								{
									found_end2 = true;
									test = false;
								}
							}
							else
							{
								return index;
							}
						}
					}
				}
				else
				{
					found_end2 = true;
				}
			}
			if (found_end1 and found_end2)
			{
				found_origin = false;
				found_plane = false;
				found_end1 = false;
				found_end2 = false;

				set_direction(static_cast<Dir>(random_static::get(0, 3)));

				update_possible_shots();
				found_ship = sf::IntRect{0, 0, 0, 0};

				return *random_static::get(possible_shots);
			}
		}
		return *random_static::get(possible_shots);
	}

	void mark_shot(sf::Vector2i index, bool is_hit) override
	{
		Player::mark_shot(index, is_hit);

		last_shot.index = index;
		last_shot.hit = is_hit;

		if (is_hit and !found_end2)
		{
			update_found_ship();
		}
	}

	void start_thinking()
	{
		delay_limit = random_static::get(20, 60);
		delay_counter = 0;
	}

	void think()
	{
		++delay_counter;
	}

	bool is_thinking() const
	{
		return delay_counter < delay_limit;
	}

private:
	struct Shot
	{
		sf::Vector2i index;
		bool hit;
		Shot() : index{0, 0}, hit{false}
		{
		}
	};

	enum class Dir
	{
		U,
		R,
		D,
		L
	};

	Shot last_shot;
	sf::Vector2i origin;
	Dir direction;
	sf::Vector2i diff;
	bool found_origin;
	bool found_plane;
	bool found_end1;
	bool found_end2;
	sf::IntRect found_ship;

	int delay_limit;
	int delay_counter;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		// for(const auto& ship : ships)
		// 	target.draw(ship, states);
		for (const auto& marker : markers)
		{
			target.draw(marker.second, states);
		}
	}

	void fill_ships() override
	{
		sf::Vector2i start{place_grid.get_bounds().left, place_grid.get_bounds().top};
		sf::IntRect bounds{defense_grid.get_bounds()};
		for (int i = 1; i <= 5; ++i)
		{
			const auto ship_size = (i < 3 ? i + 1 : i);

			ships.push_back(Ship{start, ship_size});

			do
			{
				if (random_static::get(0, 100) < 50)
				{
					ships.back().rotate();
				}

				sf::Vector2i offset{random_static::get(bounds.left, bounds.left + bounds.width),
									random_static::get(bounds.top, bounds.top + bounds.height)};

				ships.back().set_offset(offset);
			} while (!placeable(&ships.back()));
		}
	}

	void update_found_ship()
	{
		const auto index = sf::Vector2i{last_shot.index};
		if (!found_origin)
		{
			found_ship = sf::IntRect{index.x, index.y, 1, 1};
		}
		else
		{
			found_ship.left = (index.x < found_ship.left ? index.x : found_ship.left);
			found_ship.top = (index.y < found_ship.top ? index.y : found_ship.top);

			if (direction == Dir::R or direction == Dir::L)
			{
				++found_ship.width;
			}
			if (direction == Dir::U or direction == Dir::D)
			{
				++found_ship.height;
			}
		}
	}

	void update_possible_shots()
	{
		const auto rect = found_ship;

		for (int y = 0; y < rect.height + 2; ++y)
		{
			for (int x = 0; x < rect.width + 2; ++x)
			{
				sf::Vector2i index{rect.left - 1 + x, rect.top - 1 + y};
				if (possible_shots.find(index) != possible_shots.end())
					possible_shots.erase(index);
			}
		}
	}

	void set_direction(Dir d)
	{
		direction = d;
		switch (d)
		{
			case Dir::U: diff = sf::Vector2i{0, -1}; break;
			case Dir::R: diff = sf::Vector2i{1, 0}; break;
			case Dir::D: diff = sf::Vector2i{0, 1}; break;
			case Dir::L: diff = sf::Vector2i{-1, 0}; break;
			default: break;
		}
	}

	void rotate_direction()
	{
		switch (direction)
		{
			case Dir::U: set_direction(Dir::R); break;
			case Dir::R: set_direction(Dir::D); break;
			case Dir::D: set_direction(Dir::L); break;
			case Dir::L: set_direction(Dir::U); break;
			default: break;
		}
	}

	void reverse_direction()
	{
		switch (direction)
		{
			case Dir::U: set_direction(Dir::D); break;
			case Dir::R: set_direction(Dir::L); break;
			case Dir::D: set_direction(Dir::U); break;
			case Dir::L: set_direction(Dir::R); break;
			default: break;
		}
	}
	
	bool is_hit(sf::Vector2i index) const
	{
		return markers.at(index).is_hit();
	}
};
} // namespace game