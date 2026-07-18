#include "events.hpp"
#include "config.hpp"
#include "rasterizer.hpp"
#include "image.hpp"

#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include <vector>

void HandleCameraMovement(const sf::Window& window, std::vector<Camera>& cameras, int cur_index, float dt) {
    Math4D::Vector4 movement_direction = {{0.0f, 0.0f, 0.0f, 0.0f}};
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		movement_direction += Math4D::Vector4({0.0f, 1.0f, 0.0f, 0.0f});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		movement_direction += Math4D::Vector4({0.0f, -1.0f, 0.0f, 0.0f});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		movement_direction += Math4D::Vector4({1.0f, 0.0f, 0.0f, 0.0f});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		movement_direction += Math4D::Vector4({-1.0f, 0.0f, 0.0f, 0.0f});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		movement_direction += Math4D::Vector4({0.0f, 0.0f, 1.0f, 0.0f});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
		movement_direction += Math4D::Vector4({0.0f, 0.0f, -1.0f, 0.0f});
	}
	if (movement_direction != Math4D::Vector4({0.0f, 0.0f, 0.0f, 0.0f})) {
		movement_direction.Normalize();
		cameras[cur_index].Move(movement_direction * config::movement_speed * dt);
	}

	const sf::Vector2i mouse_position_px = sf::Mouse::getPosition(window);

	if (mouse_position_px.x >= 0 && mouse_position_px.x <= config::window_size.x &&
		mouse_position_px.y >= 0 && mouse_position_px.y <= config::window_size.y) {
		std::array<float, 2> mouse_position = PixelToNormal({mouse_position_px.x, mouse_position_px.y});
		const float tan_half_fov = static_cast<float>(std::tan(config::fov.asRadians() * 0.5f));
		mouse_position[0] *= config::aspect_ratio * config::near_plane_distance * tan_half_fov;
		mouse_position[1] *= config::near_plane_distance * tan_half_fov;
		
		cameras[cur_index].LookAt({{mouse_position[0], config::near_plane_distance, mouse_position[1], 0.0f}});
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(config::window_size), "Software Rasterizer");

	std::vector<Camera> cameras;
	std::vector<bool> camera_locks;

	cameras.push_back(Camera());
	camera_locks.push_back(false);
	int cur_index = 0;

	const std::vector<Triangle3D> triangles = {
		{{-5, 50, 0,
		  0, 80, 2,
		  5, 90, 0}}
	};

	std::vector<float> normal_values = Rasterize(cameras[cur_index], triangles);
	sf::Image image = NormalToGrayscale(normal_values);
	sf::Texture texture(image);
	sf::Sprite sprite(texture);

	sf::Mouse::setPosition({config::window_size.x / 2, config::window_size.y / 2}, window);
	
    sf::Clock clock;

	window.setMouseCursorVisible(false);

	while (window.isOpen())
	{
		const float dt = clock.restart().asSeconds();
		EventHandler::Events events = EventHandler::HandleEvents(window);

		if (events.close_window) {
			window.close();
		}
		if (events.create_camera && cameras.size() < 10) {
			cameras.push_back(Camera(cameras[cur_index]));
			camera_locks.push_back(false);
			cur_index = cameras.size() - 1;
		}
		if (events.change_camera != -1 && cameras.size() > events.change_camera) {
			cur_index = events.change_camera;
		}
		if (events.lock_camera) {
			camera_locks[cur_index] = !camera_locks[cur_index];
		}
		if (events.delete_camera && cameras.size() != 1) {
			for (int i = cur_index; i < cameras.size() - 1; ++i) {
				cameras[i] = std::move(cameras[i + 1]);
				camera_locks[i] = std::move(camera_locks[i + 1]);
			}
			cameras.pop_back();
			camera_locks.pop_back();
			cur_index = std::max(0, cur_index - 1);
		}

		if (!camera_locks[cur_index]) {
			HandleCameraMovement(window, cameras, cur_index, dt);
		}

		sf::Mouse::setPosition({config::window_size.x / 2, config::window_size.y / 2}, window);

		normal_values = Rasterize(cameras[cur_index], triangles);
		image = NormalToGrayscale(normal_values);
		if (!texture.loadFromImage(image)) {
			spdlog::warn("Failed to load camera's view");
			continue;
		}

		window.clear();
		window.draw(sprite);
		window.display();
	}
}
