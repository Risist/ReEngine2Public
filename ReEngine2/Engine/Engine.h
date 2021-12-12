#pragma once
#include "Common/Settings.h"
#include "Editor/Editor.h"
#include "Graphics/Renderer.h"
#include "Modes/Mode.h"

namespace engine
{

	/// TODO viewport managament
	///		* Creating new viewport
	///		* Storing all active viewports
	
	class Engine
	{
	public:
		Engine();

		template<typename _Mode = Mode, class ... ArgTypes>
		void SetActiveMode(ArgTypes&&... _args)
		{
			_activeMode = std::make_shared<_Mode>(_args...);
		}

		std::shared_ptr<Mode> GetActiveMode() const
		{
			return _activeMode;
		}

		std::shared_ptr<rendering::Viewport> GetMainViewport() const
		{
			return _mainViewport;
		}

		template<typename _Viewport, class ...Args>
		std::shared_ptr<_Viewport> CreateViewport(Args& ... args) const
		{
			static_assert(std::is_base_of<rendering::Viewport, _Viewport>::value);
			// TODO add support for managing multiple windows
			return std::make_shared<_Viewport>(args...);
		}


		void Run();

	private:
		void ProcessEvents();

		void InitWindow();

	private:
		std::shared_ptr<rendering::RenderViewport> _mainViewport;
		std::shared_ptr<Mode> _activeMode;
	};

	extern std::unique_ptr<Engine> engine;
}