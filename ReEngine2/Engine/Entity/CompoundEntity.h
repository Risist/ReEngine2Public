#pragma once
#include "Entity.h"

namespace entity
{
	class CompoundEntity : public Entity, public EntityContainer
	{
		RTTI_DECLARE_ASSET(CompoundEntity);
	public:
		CompoundEntity() = default;
		CompoundEntity(const CompoundEntity& other)
			: Entity(other)
			// do not use copy constructor cause it works incorrectly when _this refference was not yet set up
			// Use duplicate method instead, to get a copy with copied children
			, EntityContainer() 
		{
			EntityContainer::SetCurrentLevel(other.GetLevel());
		}

		/// /brief Creates a new component Entity
		///
		/// Creates component using it's constructor with @param _args parameters
		/// After creation inserts to the container
		/// Sets asset name to the @param name 
		template<class Ent = Entity, class ... ArgTypes>
		std::shared_ptr<Ent> CreateComponent(const std::string& name, ArgTypes&&... _args)
		{
			auto shared = EntityContainer::CreateComponent<Ent>(name, _args...);
			shared->SetParent(_this);
			return shared;
		}

		/// /brief inserts already created Entity to the container and claims an ownership
		template<class Ent = Entity>
		std::shared_ptr<Ent> InsertComponent(std::shared_ptr<Ent> component)
		{
			auto shared = EntityContainer::InsertComponent<Ent>(component);
			shared->SetParent(_this);
			return shared;
		}

		virtual void OnUpdate(sf::Time dt) override
		{
			Entity::OnUpdate(dt);
			for (auto& it : *this)
			{
				it->OnUpdate(dt);
			}
		}

		virtual void OnFixedUpdate(sf::Time dt) override
		{
			Entity::OnFixedUpdate(dt);
			for (auto& it : *this)
			{
				it->OnFixedUpdate(dt);
			}
		}

		virtual void OnDraw(rendering::RenderViewport& viewport) override
		{
			Entity::OnDraw(viewport);
			for (auto& it : *this)
			{
				it->OnDraw(viewport);
			}
		}

	public: // Editor

		virtual bool CanBeAChild(const rtti::ClassType& potentialChildType) const;

		virtual std::shared_ptr<Entity> Duplicate() const;

	protected:
		virtual void OnInit() override
		{
			SetCurrentLevel(_currentLevel);
		}

		virtual void OnSpawn() override
		{
			Entity::OnSpawn();
			for (auto& it : *this)
			{
				it->Spawn();
			}
		}

		virtual void OnEnable() override
		{
			Entity::OnEnable();
			for (auto& it : *this)
			{
				it->Enable();
			}
		}

		virtual void OnDisable() override
		{
			Entity::OnDisable();
			for (auto& it : *this)
			{
				it->Disable();
			}
		}

		virtual void OnDestroy() override
		{
			Entity::OnDestroy();
			for (auto& it : *this)
			{
				it->Destroy();
			}
		}

		virtual void OnVisibilityChanged() override
		{
			Entity::OnVisibilityChanged();
		}

	private:

	};


}