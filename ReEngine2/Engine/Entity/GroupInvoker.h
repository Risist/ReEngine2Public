#pragma once
#include "Entity.h"

namespace entity
{
	template<class Type>
	class GroupInvokerBase
	{
	public:

		void RegisterObject(std::weak_ptr<Type> s)
		{
			invokeObjects.push_back(s);
		}

	protected:
		void RemoveInvalidReferences()
		{
			for (int i = (int)invokeObjects.size() - 1; i >= 0; --i)
			{
				if (invokeObjects[i].expired())
				{
					invokeObjects.erase(invokeObjects.begin() + i);
				}
			}
		}

		std::vector<std::weak_ptr<Type>> invokeObjects;
	};

	class GroupInvoker_Update : public GroupInvokerBase<Entity>
	{
	public:

		void Update()
		{
			RemoveInvalidReferences();

			for (int i = 0; i < invokeObjects.size(); ++i)
			{
				auto obj = invokeObjects[i].lock();
				if (obj->IsEnabled())
				{
					obj->OnUpdate(timer.getElapsedTime());
				}
			}

			timer.restart();
		}

	private:
		sf::Clock timer;
	};

	class GroupInvoker_FixedUpdate : public GroupInvokerBase<Entity>
	{
	public:
		GroupInvoker_FixedUpdate(sf::Time fixedDeltaTime = sf::seconds(0.1f))
			: fixedDeltaTime(fixedDeltaTime)
		{}

		void FixedUpdate()
		{
			RemoveInvalidReferences();

			timeStorage += timer.getElapsedTime();

			while (timeStorage >= fixedDeltaTime)
			{
				timeStorage -= fixedDeltaTime;

				for (int i = 0; i < invokeObjects.size(); ++i)
				{
					auto obj = invokeObjects[i].lock();
					if (obj->IsEnabled())
					{
						obj->OnFixedUpdate(fixedDeltaTime);
					}
				}
			}

			timer.restart();
		}

	private:
		sf::Time timeStorage;

		sf::Time fixedDeltaTime;
		sf::Clock timer;
	};

	class GroupInvoker_DrawLayer : public GroupInvokerBase<Entity>
	{
	public:

		void Draw(rendering::RenderViewport& viewport)
		{
			RemoveInvalidReferences();

			for (int i = 0; i < invokeObjects.size(); ++i)
			{
				auto obj = invokeObjects[i].lock();
				if (obj->IsEnabled() && obj->IsVisible())
				{
					obj->OnDraw(viewport);
				}
			}
		}
	};

	class GroupInvoker_Draw
	{
	public:
		GroupInvoker_Draw(int numLayers = 1)
		{
			displayLayers.resize(numLayers);
		}

		void Draw(rendering::RenderViewport& viewport)
		{
			for (int i = 0; i < displayLayers.size(); ++i)
			{
				displayLayers[i].Draw(viewport);
			}
		}

		void RegisterObject(std::weak_ptr<Entity> s, int layerId)
		{
			displayLayers[layerId].RegisterObject(s);
		}

	private:
		std::vector<GroupInvoker_DrawLayer> displayLayers;
	};
}