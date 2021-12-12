#pragma once
#include <queue>
#include <memory>
#include "../Common/Settings.h"
#include "../Common/Math/Math.h"

namespace rendering
{

	class RenderObject;

	// for now main window managament
	class Viewport
	{
	public:

		void ProcessEvents()
		{
			sf::Event event;
			while (_window.pollEvent(event))
			{
				//ImGui::SFML::ProcessEvent(event);
				if (event.type == sf::Event::Closed)
					_window.close();

				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					_window.close();
			}
		}

		void InitWindow()
		{
			_window.create(sf::VideoMode(1800, 1080), "ReEngine");
		}

		sf::RenderWindow _window;
	};

	// some render target for rendering
	class RenderViewport : public Viewport
	{
	public:

	};
}