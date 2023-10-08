#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

const float speed = 6.0f;
const float mouseSensitivity = 150.0f;

std::string ReadTextFile(const std::string& fileName);

int main(int argc, char** argv)
{
	sf::ContextSettings settings;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.depthBits = 24;
	settings.attributeFlags = sf::ContextSettings::Default;
	sf::Window window(sf::VideoMode(800, 800), "3D OpenGL", sf::Style::Default, settings);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initalize GLEW\n";
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	std::string fileName = "scene.fbx";
	if (argc > 1)
		fileName = argv[1];

	Shader shader(ReadTextFile("vertex.glsl"), ReadTextFile("fragment.glsl"));
	Scene scene(fileName);

	Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));

	shader.Use();
	shader.SetValue("ambientStrength", 0.1f);
	shader.SetValue("lightColor", glm::vec3(1.0f));

	bool isFirstMouse = true;
	sf::Vector2i lastMousePos{};

	sf::Clock clock{};
	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		sf::Event event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				glViewport(0, 0, window.getSize().x, window.getSize().y);
		}

		camera.UpdateDrectionVectors();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			camera.position += camera.Forward() * speed * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			camera.position -= camera.Forward() * speed * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			camera.position -= camera.Right() * speed * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			camera.position += camera.Right() * speed * deltaTime;

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			if (isFirstMouse)
			{
				lastMousePos = sf::Mouse::getPosition(window);
				isFirstMouse = false;
				window.setMouseCursorVisible(false);
			}
			else
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int xOffset = mousePos.x - lastMousePos.x;
				int yOffset = lastMousePos.y - mousePos.y;

				camera.yaw += xOffset * mouseSensitivity * deltaTime;
				camera.pitch += yOffset * mouseSensitivity * deltaTime;

				sf::Mouse::setPosition(lastMousePos, window);
			}
		}
		else
		{
			isFirstMouse = true;
			window.setMouseCursorVisible(true);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		shader.SetValue("view", camera.GetViewMatrix());
		shader.SetValue("projection", camera.GetProjectionMatrix((float)window.getSize().x,
			(float)window.getSize().y));
		shader.SetValue("lightPos", camera.position);
		shader.SetValue("viewPos", camera.position);

		scene.Draw(shader);

		window.display();
	}
}

std::string ReadTextFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
		return "";

	std::stringstream ss{};
	ss << file.rdbuf();
	file.close();

	return ss.str();
}
