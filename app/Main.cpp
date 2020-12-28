#include "common/Common.hpp"
#include "game/Game.hpp"
#include "game/Grid.hpp"
#include "game/Players.hpp"
#include "gui/Button.hpp"
#include "gui/Endscreen.hpp"
#include "gui/Particles.hpp"
#include "gui/Sfline.hpp"
#include "util/Sfutils.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <memory>

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window{sf::VideoMode{common::window_size.x, common::window_size.y}, "Battleship Game", sf::Style::Default, settings};
	window.setFramerateLimit(60);
	sf::Color background{5, 20, 43};

	if (!common::font.loadFromFile("../fonts/consola.ttf"))
	{
		return EXIT_FAILURE;
	}

	std::unique_ptr<game::Game> simulation{std::make_unique<game::Game>()};

	while (window.isOpen())
	{
		sf::Vector2i mousei{sf::Mouse::getPosition(window)};
		sf::Vector2f mouse{mousei.x * 1.0f, mousei.y * 1.0f};

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					simulation->lmb_press(mouse);

					if (simulation->reset())
					{
						simulation = std::make_unique<game::Game>();
					}
				}
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					simulation->rmb_press(mouse);
				}
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					simulation->spacebar_press();
				}
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
			}
		}

		simulation->hover(mouse);
		simulation->update();

		window.clear(background);
		window.draw(*simulation);
		window.display();
	}

	return 0;
}
