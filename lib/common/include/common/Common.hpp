#include "SFML/Graphics.hpp"

extern const sf::Vector2u g_winsize;
extern const sf::Vector2i g_cellcount;
extern const float g_cellsize;
extern sf::Font g_font;

extern sf::Vector2i screen_position_to_index(sf::Vector2f pos);
extern sf::Vector2f index_to_screen_position(sf::Vector2i index);
extern sf::Vector2f index_to_center_position(sf::Vector2i index);
