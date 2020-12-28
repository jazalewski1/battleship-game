#include "common/Common.hpp"
#include "SFML/System.hpp"

namespace common
{
sf::Vector2i screen_position_to_index(sf::Vector2f pos)
{
	return sf::Vector2i{static_cast<int>(pos.x / cell_size), static_cast<int>(pos.y / cell_size)};
}

sf::Vector2f index_to_screen_position(sf::Vector2i index)
{
	return sf::Vector2f{index.x * cell_size, index.y * cell_size};
}

sf::Vector2f index_to_center_position(sf::Vector2i index)
{
	return sf::Vector2f{(index.x * cell_size) + (cell_size * 0.5f), (index.y * cell_size) + (cell_size * 0.5f)};
}
} // namespace common