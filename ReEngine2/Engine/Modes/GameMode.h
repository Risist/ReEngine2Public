#pragma once
#include "Mode.h"
#include "../Entity/EntityCore.h"
#include "../Graphics/Rendering.h"

class Creature : public entity::CompoundEntity
{
	RTTI_DECLARE_ASSET(Creature);
public:

	virtual void OnInit() override
	{
		entity::CompoundEntity::OnInit();
	}

	virtual void OnSpawn() override
	{
		entity::CompoundEntity::OnSpawn();
		auto locked = _currentLevel.lock();
		locked->RegisterForDisplay(_this, 0);
		locked->RegisterForUpdate(_this);
		locked->RegisterForFixedUpdate(_this);
	}

	virtual void OnUpdate(sf::Time dt)
	{
		entity::CompoundEntity::OnUpdate(dt);
		//std::cout << GetName();
	}

	float x;
	std::weak_ptr<rendering::SpriteAsset> sprite;
};


namespace engine
{
	class GameMode : public Mode
	{

	public:
		virtual void Init() override
		{
			std::shared_ptr<entity::Level> level = entity::world->CreateNewLevel();
			entity::world->SetCurrentLevel(level);

			SpawnInitialEntities(level);

			entity::world->SetCurrentLevel(level);
			entity::world->StartCurrentLevel();
		}

		virtual void CleanUp() override 
		{
			entity::world->StopCurrentLevel();
		}

		virtual void ProcessFrame() override
		{
			entity::world->ProcessFrame();
		}

		virtual void Display(rendering::RenderViewport& viewport) override
		{
			entity::world->Display(viewport);
		}

	protected:

		virtual void SpawnInitialEntities(std::shared_ptr<entity::Level> level)
		{
			auto entity = level->CreateEntity<Creature>("Meh");

			entity->CreateComponent<Creature>("Another");
		}
	};
}