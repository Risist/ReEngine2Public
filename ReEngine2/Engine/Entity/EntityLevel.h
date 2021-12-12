#pragma once
#include "Entity.h"
#include "GroupInvoker.h"

namespace entity
{
	enum class ELevelState
	{
		ECreated = 0,
		EInitializing,
		EInitialized,
		EStarting,
		EStarted,
		EStopping,
		EStoped
	};

	class Level : public Asset
	{
		RTTI_DECLARE_ASSET(Level);
	public:
		Level()
			: _levelState(ELevelState::ECreated)
		{

		}

		void Init(std::weak_ptr<Level> _this)
		{
			_levelState = ELevelState::EInitializing;
			this->_this = _this;

			_levelState = ELevelState::EInitialized;
		}

		std::shared_ptr<Level> Duplicate() const
		{
			std::shared_ptr<Level> copy = std::make_shared<Level>();
			copy->Init(copy);

			for (const auto& it : _entitiesToAdd)
			{
				copy->InsertEntity(it->Duplicate());
			}

			for (const auto& it : _entities)
			{
				copy->InsertEntity(it->Duplicate());
			}

			return copy;
		}

		void Start()
		{
			RE_ASSERT(_levelState == ELevelState::EInitialized);
			_levelState = ELevelState::EStarting;

			RemovePeendingReferences();
			for (auto& it : _entities)
			{
				it->Spawn();
			}
			InsertPeendingReferences();



			_levelState = ELevelState::EStarted;
		}

		// clean up the whole scene
		void Stop()
		{
			RE_ASSERT(_levelState == ELevelState::EStarted);
			InsertPeendingReferences();

			_levelState = ELevelState::EStopping;
			for (auto& it : _entities)
			{
				it->Destroy();
			}

			RemovePeendingReferences();
			RE_ASSERT(_entitiesToAdd.empty());//, "You should not add any entities during OnDestroy event");

			_levelState = ELevelState::EStoped;
		}

		void ProcessFrame()
		{
			RemovePeendingReferences();
			InsertPeendingReferences();

			_updateInvoker.Update();
			_fixedUpdateInvoker.FixedUpdate();
		}


		void Display(rendering::RenderViewport& viewport)
		{
			_displayInvoker.Draw(viewport);
		}

		/// add entity
		/// remove entity
		// Duplicate - for editor usage 
		/// creates new component and then adds to world actor list

		template<class Ent = Entity, class ... ArgTypes>
		std::shared_ptr<Ent> CreateEntity(const std::string& name, ArgTypes&&... _Args)
		{
			auto act = std::make_shared<Ent>(std::forward<ArgTypes>(_Args)...);
			act->SetName(name);
			auto weak = InsertEntity<Ent>(act);
			return weak;
		}

		template<class Ent = Entity>
		std::shared_ptr<Ent> InsertEntity(std::shared_ptr<Ent> entity)
		{
			if (_levelState == ELevelState::EStopping
				|| _levelState == ELevelState::EStoped)
				return std::shared_ptr<Ent>(); // can't add an entity if we are stopping the level

			/// registration should happen when spawn happens ?
			// for update -> yes
			// for draw -> no, we need to register it instantly
			// register for update
			// register for draw

			entity->_this = entity;
			entity->_currentLevel = _this;
			entity->Init();

			std::shared_ptr<Ent> weak = entity;
			_entitiesToAdd.push_back(std::move(entity));
			return weak;
		}

		template<class Ent = Entity>
		void RemoveEntityImidiate(std::shared_ptr<Ent> entity)
		{
			RE_ASSERT(entity->GetState() != EEntityState::EEnabling);
			RE_ASSERT(entity->GetState() != EEntityState::EDisabling);
			RE_ASSERT(entity->GetState() != EEntityState::ESpawning);
			RE_ASSERT(entity->GetState() != EEntityState::EInitializing);

			if (entity->IsEnabled())
			{
				entity->Disable();
			}

			entity->PerformDestroy();
		}


		void RegisterForUpdate(std::weak_ptr<Entity> updatable)
		{
			_updateInvoker.RegisterObject(updatable);
		}
		void RegisterForFixedUpdate(std::weak_ptr<Entity> updatable)
		{
			_fixedUpdateInvoker.RegisterObject(updatable);
		}
		void RegisterForDisplay(std::weak_ptr<Entity> displayable, int layer)
		{
			_displayInvoker.RegisterObject(displayable, 0);
		}

		ELevelState GetLevelState() const { return _levelState; }

	public:

		auto begin() { return _entities.begin(); }
		auto end() { return _entities.end(); }
		auto cbegin() const { return _entities.begin(); }
		auto cend() const { return _entities.end(); }
		auto begin() const { return _entities.begin(); }
		auto end() const { return _entities.end(); }

		std::shared_ptr<Entity> FindEntity(Guid guid)
		{
			for (auto it : _entities)
			{
				if (it->GetGuid() == guid)
				{
					return it;
				}
			}

			return std::shared_ptr<Entity>();
		}

		std::shared_ptr<Entity> FindEntityInDepth(Guid guid)
		{
			std::shared_ptr<Entity> rVal;
			IterateInDepth([guid, &rVal](std::shared_ptr<Entity> entity)
				{
					if (entity->GetGuid() == guid)
					{
						rVal = entity;
						return true;
					}
					return false;
				});

			return rVal;
		}

		bool IterateInDepth(std::function<bool(std::shared_ptr<Entity>)> operation) const
		{
			for (auto it : _entities)
			{
				if (operation(it))
				{
					return true;
				}

				auto compound = dynamic_cast<EntityContainer*>(it.get());
				if (compound != nullptr)
				{
					if(compound->IterateInDepth(operation))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool Iterate(std::function<bool(std::shared_ptr<Entity>)> operation) const
		{
			for (auto it : _entities)
			{
				if (operation(it))
				{
					return true;
				}
			}

			return false;
		}

	private:
		void RemovePeendingReferences()
		{
			for (int i = (int)_entities.size() - 1; i >= 0; --i)
			{
				// remove references children if compound first
				auto compound = dynamic_cast<EntityContainer*>(_entities[i].get());
				if (compound)
				{
					compound->RemovePeendingReferences();
				}

				if (_entities[i]->GetEntityState() == EEntityState::EPeendingDestroy)
				{
					_entities[i]->PerformDestroy();
					_entities.erase(_entities.begin() + i);
				}
			}
		}
		void InsertPeendingReferences()
		{
			for (int i = 0; i < _entitiesToAdd.size(); ++i)
			{
				_entitiesToAdd[i]->Spawn();
				_entities.push_back(std::move(_entitiesToAdd[i]));
			}
			_entitiesToAdd.clear();
		}

	private:
		GroupInvoker_Update			_updateInvoker;
		GroupInvoker_FixedUpdate	_fixedUpdateInvoker;
		GroupInvoker_Draw			_displayInvoker;

		std::vector<std::shared_ptr<Entity>> _entities;
		std::vector<std::shared_ptr<Entity>> _entitiesToAdd;

		std::weak_ptr<Level> _this;

		ELevelState _levelState;
	};

}