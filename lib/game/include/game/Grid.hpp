#ifndef GRID_HPP
#define GRID_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Cell.hpp"
#include "util/Sfutils.hpp"
#include <unordered_map>

namespace game
{
class Grid : public sf::Drawable
{
public:
	Grid(sf::Vector2i offset, sf::Vector2i size) :
		offset{offset}, size{size}, bounds{offset, size}
	{
		for (int y = 0; y < size.y; ++y)
		{
			for (int x = 0; x < size.x; ++x)
			{
				const auto index = sf::Vector2i{offset.x + x, offset.y + y};
				cells.emplace(index, index);
			}
		}
	}

	Grid(int offset_x, int offset_y, int size_x, int size_y) :
		Grid{sf::Vector2i{offset_x, offset_y}, sf::Vector2i{size_x, size_y}}
	{
	}

	virtual bool contains(sf::Vector2i index) const
	{
		return bounds.contains(index);
	}

	virtual bool contains(sf::Vector2f pos) const
	{
		return contains(common::screen_position_to_index(pos));
	}

	sf::IntRect get_bounds() const
	{
		return bounds;
	}

	Cell* get_cell(sf::Vector2i index)
	{
		const auto search = cells.find(index);
		if (search != cells.end())
		{
			return &search->second;
		}
		return nullptr;
	}

	Cell* get_cell(sf::Vector2f pos)
	{
		return get_cell(common::screen_position_to_index(pos));
	}

protected:
	std::unordered_map<sf::Vector2i, Cell> cells; // TODO: initialize with a free function (when moved to .cpp) and change to const
	const sf::Vector2i offset;
	const sf::Vector2i size;
	const sf::IntRect bounds;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (const auto& elem : cells)
		{
			target.draw(elem.second, states);
		}
	}
};

class GridLabeled : public Grid
{
public:
	GridLabeled(sf::Vector2i offset, sf::Vector2i size) :
		Grid{offset, size}
	{
		for (int x = 0; x < size.x; ++x)
		{
			const auto index = sf::Vector2i{offset.x + x, offset.y - 1};
			const auto symbol = static_cast<char>(48 + x);
			labels.emplace(index, LabelCell{index, symbol});
		}
		for (int y = 0; y < size.y; ++y)
		{
			const auto index = sf::Vector2i{offset.x - 1, offset.y + y};
			const auto symbol = static_cast<char>(65 + y);
			labels.emplace(index, LabelCell{index, symbol});
		}
	}

	GridLabeled(int offset_x, int offset_y, int size_x, int size_y) :
		GridLabeled{sf::Vector2i{offset_x, offset_y}, sf::Vector2i{size_x, size_y}}
	{
	}

	std::pair<char, char> get_symbols(sf::Vector2i index) const
	{
		const auto search_x = labels.find(sf::Vector2i{index.x, offset.y - 1});
		const auto search_y = labels.find(sf::Vector2i{offset.x - 1, index.y});
		if (search_x != labels.end() && search_y != labels.end())
		{
			return std::make_pair(search_x->second.get_symbol(), search_y->second.get_symbol());
		}
		else
		{
			return std::make_pair('?', '?');
		}
	}

private:
	std::unordered_map<sf::Vector2i, LabelCell> labels;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		Grid::draw(target, states);
		for (const auto& cell : labels)
		{
			target.draw(cell.second, states);
		}
	}
};
} // namespace game
#endif