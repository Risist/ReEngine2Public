#pragma once
#include "EntityLevel.h"

namespace entity
{

	/// Responsibilities: 
	///		* managing levels
	///		* managing updates
	class World
	{
	public:

		std::shared_ptr<Level> CreateNewLevel()
		{
			std::shared_ptr<Level> shared = std::make_shared<Level>();
			shared->Init(shared);
			return shared;
		}

		void SetCurrentLevel(std::shared_ptr<Level> level)
		{
			RE_ASSERT(!_currentLevel ||
				(_currentLevel->GetLevelState() != ELevelState::EStarting
					&& _currentLevel->GetLevelState() != ELevelState::EStarted)
			);

			_currentLevel = std::move(level);
			_currentLevel->Init(_currentLevel);
		}

		std::shared_ptr<Level> GetCurrentLevel() const
		{
			return _currentLevel;
		}


		bool StartCurrentLevel()
		{
			if (_currentLevel == nullptr)
				return false;

			_currentLevel->Start();


			return true;
		}
		void StopCurrentLevel()
		{
			if (_currentLevel == nullptr)
				return;

			_currentLevel->Stop();

			_currentLevel = nullptr;
		}

		void ProcessFrame()
		{
			if (_currentLevel == nullptr)
				return;

			_currentLevel->ProcessFrame();
		}

		void Display(rendering::RenderViewport& viewport)
		{
			if (_currentLevel == nullptr)
				return;

			_currentLevel->Display(viewport);
		}

	public:
		auto begin() { return _currentLevel->begin(); }
		auto end() { return _currentLevel->end(); }
		auto cbegin() const { return _currentLevel->begin(); }
		auto cend() const { return _currentLevel->end(); }
		auto begin() const { return _currentLevel->begin(); }
		auto end() const { return _currentLevel->end(); }

		// TODO
		// get entity by guid from top level
		// get entity by guid with nested ones

		std::shared_ptr<Entity> FindEntity(Guid guid)
		{
			return _currentLevel->FindEntity(guid);
		}
		std::shared_ptr<Entity> FindEntityInDepth(Guid guid)
		{
			return _currentLevel->FindEntityInDepth(guid);
		}



		bool IterateInDepth(std::function<bool(std::shared_ptr<Entity>)> operation) const
		{
			return _currentLevel->IterateInDepth(operation);
		}

		bool Iterate(std::function<bool(std::shared_ptr<Entity>)> operation) const
		{
			return _currentLevel->Iterate(operation);
		}

	private:
		std::shared_ptr<Level> _currentLevel;
	};


	extern std::unique_ptr<World> world;
}