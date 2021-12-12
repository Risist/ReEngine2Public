#pragma once
#include "GameMode.h"
#include "../Editor/Editor.h"
#include "../Graphics/Renderer.h"

namespace engine
{

	class EditorMode : public GameMode
	{
	public:

		virtual void Init() override
		{
			auto& window = engine::engine->GetMainViewport()->_window;

			// create editor
			editor::editor = std::make_unique<editor::Editor>();
			editor::editor->Init();

			// init level
			std::shared_ptr<entity::Level> level = entity::world->CreateNewLevel();
			entity::world->SetCurrentLevel(level);

			SpawnInitialEntities(level);

			entity::world->SetCurrentLevel(level->Duplicate());
			entity::world->StartCurrentLevel();
		}

		virtual void ProcessEvent(sf::Event event) override
		{
			auto& window = engine::engine->GetMainViewport()->_window;
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		virtual void ProcessFrame() override
		{
			auto& window = engine::engine->GetMainViewport()->_window;
			ImGui::SFML::Update(window, deltaClock.restart());
			entity::world->ProcessFrame();
		}

		virtual void Display(rendering::RenderViewport& viewport) override
		{
			editor::editor->Draw();
			entity::world->Display(viewport);

			ImGui::SFML::Render(viewport._window);
			imGuiManager.ResetId();
		}

	private:
		sf::Clock deltaClock;
	};
}