#pragma once
#include "../Reflection/Reflection.h"
#include <Engine/AssetManagament/Asset.h>


namespace rendering
{
	class RenderViewport;
}

namespace entity
{
	class Entity;
	class CompoundEntity;
	class Level;
	class World;
	class Transform;

	enum class EEntityState : uint8
	{
		ECreated = 0,

		EInitializing,
		EInitialized,

		ESpawning,

		EEnabling,
		EEnabled,

		EDisabling,
		EDisabled,

		EPeendingDestroy,

		EDestroying,
		EDestroyed,
	};


	class Entity : public Asset//: public ComponentContainer
	{
		RTTI_DECLARE_ASSET(Entity);
	public:
		Entity();

	public: // entity state modifying functions

		void Init();

		void Spawn();

		void Enable();

		void Disable();

		void SetEnabled(bool enabled);

		void Destroy();

		void SetVisibility(bool visible);


		bool IsInitialized() const
		{
			return _entityState >= EEntityState::EInitialized;
		}
		bool IsEnabled() const
		{
			return _entityState == EEntityState::EEnabled;
		}
		bool IsDisabled() const
		{
			return _entityState >= EEntityState::EDisabled;
		}
		bool IsDestroyed() const
		{
			return _entityState >= EEntityState::EPeendingDestroy;
		}
		bool IsVisible() const { return _bVisible; }

		EEntityState GetEntityState() const
		{
			return _entityState;
		}

	public:

		virtual void OnUpdate(sf::Time dt) {}

		virtual void OnFixedUpdate(sf::Time dt) {}

		virtual void OnDraw(rendering::RenderViewport& viewport) {}

	protected:
		/// /brief event called once right after the level is started
		///
		/// Called in editor or while playout
		virtual void OnInit() {}

		/// /brief event called when map is loaded or at first frame
		virtual void OnSpawn() {}

		virtual void OnEnable() {}

		virtual void OnDisable() {}

		virtual void OnDestroy() {}

		virtual void OnVisibilityChanged() {}

	public:

		std::weak_ptr<Level> GetLevel() const { return _currentLevel; }
		void SetLevel(std::weak_ptr<Level> level) { _currentLevel = level; }

		std::weak_ptr<Entity> GetParent() const { return _parent; }
		void SetParent(std::weak_ptr<Entity> parent) {  _parent = parent; }
		
	public: // Editor
		static constexpr const char* ASSET_PAYLOAD_CODE = "Asset_Entity";
		static constexpr const char* SCENE_PAYLOAD_CODE = "Scene_Entity";

		virtual void ShowInInspector(class editor::EditorContext& editorContext) override;
		virtual void CreateDragPayload() const override;
		virtual bool CanBeChildOf(class CompoundEntity* potentialParent);

		virtual std::shared_ptr<Entity> Duplicate() const;

	protected:
		std::weak_ptr<Entity> _parent;
		std::weak_ptr<Entity> _this;
		std::weak_ptr<Level> _currentLevel;

	private:
		
		EEntityState _entityState : 4;
		bool _bVisible : 1;
		
		void PerformDestroy();

		friend class Level;
		friend class EntityContainer;
		friend class CompoundEntity;
	};


	class EntityContainer
	{
	public:
		EntityContainer() = default;
		EntityContainer(const EntityContainer& other) noexcept
		{
			_level = other._level;
			for (const auto& it : other._components)
			{
				InsertComponent(it->Duplicate());
			}
		}

		/// /brief Creates a new component Entity
		///
		/// Creates component using it's constructor with @param _args parameters
		/// After creation inserts to the container
		/// Sets asset name to the @param name 
		template<class Ent = Entity, class ... ArgTypes>
		std::shared_ptr<Ent> CreateComponent(const std::string& name, ArgTypes&&... _args)
		{
			static_assert(std::is_base_of<Entity, Ent>::value);

			auto shared = std::make_shared<Ent>(_args...);
			shared->SetName(name);

			return InsertComponent<Ent>(shared);
		}

		/// /brief inserts already created Entity to the container and claims an ownership
		template<class Ent = Entity>
		std::shared_ptr<Ent> InsertComponent(std::shared_ptr<Ent> component)
		{
			static_assert(std::is_base_of<Entity, Ent>::value);
			component->_this = component;
			component->_currentLevel = _level;
			component->Init();

			_components.push_back(component);

			return component;
		}

		bool RemoveComponent(std::weak_ptr<Entity> component)
		{
			auto locked = component.lock();
			auto found = std::find(_components.begin(), _components.end(), locked);
			if (found == _components.end())
			{
				return false;
			}

			// call some cleaning up function
			//found->get();

			_components.erase(found);

			return true;
		}

		/// /brief returns first found component of a given class type
		///
		/// (Static) Asserts if Comp is not an Entity derived type
		template<class Ent>
		std::weak_ptr<Ent> FindComponent() const
		{
			static_assert(std::is_base_of<Entity, Ent>::value);
			for (auto& it : _components)
			{
				if (dynamic_cast<Ent>(it.get()) != nullptr)
				{
					return std::weak_ptr<Ent>(it);
				}
			}

			return std::weak_ptr<Ent>();
		}

		/// /brief returns first found component of a given class type
		///
		/// (Runtime) Asserts if classType is not an Entitty derived type
		std::weak_ptr<Entity> FindComponent(rtti::ClassType* classType) const
		{
			RE_ASSERT(classType->IsA<Entity>());
			for (auto& it : _components)
			{
				if (classType->IsA(rtti::GetClassDescriptor<Entity>()))
				{
					return std::weak_ptr<Entity>(it);
				}
			}

			return std::weak_ptr<Entity>();
		}

		/// /brief returns first found component with a given guid
		///
		/// (Static) Asserts if Ent is not an Entity derived type
		template<class Ent = Entity>
		std::weak_ptr<Ent> FindComponent(Guid guid) const
		{
			static_assert(std::is_base_of<Entity, Ent>::value);
			for (auto& it : _components)
			{
				if (dynamic_cast<Ent>(it.get()) != nullptr)
				{
					return std::weak_ptr<Ent>(it);
				}
			}

			return std::weak_ptr<Ent>();
		}

		void RemovePeendingReferences()
		{
			for (int i = (int)_components.size() - 1; i >= 0; --i)
			{
				// remove references children if compound first
				auto compound = dynamic_cast<EntityContainer*>(_components[i].get());
				if (compound)
				{
					compound->RemovePeendingReferences();
				}

				if (_components[i]->GetEntityState() == EEntityState::EPeendingDestroy)
				{
					_components[i]->PerformDestroy();
					_components.erase(_components.begin() + i);
				}
			}
		}

	public:

		auto begin() { return _components.begin(); }
		auto end() { return _components.end(); }
		auto cbegin() const { return _components.begin(); }
		auto cend() const { return _components.end(); }
		auto begin() const { return _components.begin(); }
		auto end() const { return _components.end(); }

		bool HasAnyChildComponent() const { return !_components.empty(); }

		std::shared_ptr<Entity> FindEntity(Guid guid)
		{
			for (auto it : _components)
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
			for (auto it : _components)
			{
				if (operation(it))
				{
					return true;
				}

				auto compound = dynamic_cast<EntityContainer*>(it.get());
				if (compound != nullptr)
				{
					if (compound->IterateInDepth(operation))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool Iterate(std::function<bool(std::shared_ptr<Entity>)> operation) const
		{
			for (auto it : _components)
			{
				if (operation(it))
				{
					return true;
				}
			}

			return false;
		}

	protected:
		std::vector<std::shared_ptr<Entity>> _components;

		void SetCurrentLevel(std::weak_ptr<Level> currentLevel)
		{
			_level = currentLevel;
		}

	private:
		std::weak_ptr<Level> _level;
	};
}

namespace editor
{
	class EntityAssetDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;
	};
}

RTTI_DEFINE_EXTERNAL_CLASS(std::weak_ptr<class Entity>,
	{
		RTTI_DRAWER(editor::EntityAssetDrawer);
		//TODO serializer
		//		scene reference vs assetReference

		RTTI_DISPLAY_NAME("Entity");
	});