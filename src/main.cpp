#include "events.hpp"
#include "config.hpp"
#include "rasterizer.hpp"
#include "image.hpp"

#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(config::window_size), "Software Rasterizer");

	const auto main_camera = Camera();

	const std::vector<Triangle3D> triangles = {
		{{-5, 5, 0,
		  0, 8, 2,
		  5, 10, 0}}
	};

	std::vector<float> normal_values = Rasterize(main_camera, triangles);
	sf::Image image = NormalToGrayscale(normal_values);
	sf::Texture texture(image);
	sf::Sprite sprite(texture);


	while (window.isOpen())
	{
		EventHandler::Events events = EventHandler::HandleEvents(window);

		if (events.close_window) {
			window.close();
		}

		window.clear();
		window.draw(sprite);
		window.display();
	}
}
