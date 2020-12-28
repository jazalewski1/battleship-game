#pragma once
#include "SFML/Graphics.hpp"

namespace common
{
const sf::Vector2i grid_size{22, 28};
const float cell_size{32.0f};
const sf::Vector2u window_size{static_cast<unsigned int>(grid_size.x * cell_size), static_cast<unsigned int>(grid_size.y *cell_size)};
const int single_board_size{10};
sf::Font font;

sf::Vector2i screen_position_to_index(sf::Vector2f pos);
sf::Vector2f index_to_screen_position(sf::Vector2i index);
sf::Vector2f index_to_center_position(sf::Vector2i index);
} // namespace common