#include <iostream>

#include <Engine/Engine.h>

#include <Engine/Graphics/Renderer.h>
#include <Engine/Modes/EditorMode.h>

int main()
{
	engine::engine = std::make_unique<engine::Engine>();
	engine::engine->SetActiveMode<engine::EditorMode>();

	engine::engine->Run();

	return 0;
}