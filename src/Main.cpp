#include <SFML/Graphics.hpp>

const sf::Vector2u winsize (640, 480);

int main()
{
	sf::RenderWindow window (sf::VideoMode(winsize.x, winsize.y), "Template title");

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.display();
	}

	return 0;
}
