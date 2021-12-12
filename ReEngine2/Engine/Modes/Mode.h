#pragma once
#include "../Graphics/Renderer.h"

namespace engine
{
	class Mode
	{
	public:

		// Initialize mode 
		virtual void Init() = 0;

		virtual void CleanUp() { }

		virtual void ProcessEvent(sf::Event event) {}

		virtual void ProcessFrame() {}

		virtual void Display(rendering::RenderViewport& viewport) {}
	};
}