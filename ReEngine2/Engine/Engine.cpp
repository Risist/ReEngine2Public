#include "Engine.h"

namespace engine
{
    std::unique_ptr<Engine> engine;

    Engine::Engine()
    {
    }

    void Engine::Run()
	{
        RE_ASSERT(_activeMode);

        InitWindow();
        _activeMode->Init();

        sf::Clock deltaClock;
        while (_mainViewport->_window.isOpen())
        {
            ProcessEvents();

            _activeMode->ProcessFrame();

            // display
            _mainViewport->_window.clear();
            _activeMode->Display(*_mainViewport);
            _mainViewport->_window.display();
        }

        _activeMode->CleanUp();
	}

    void Engine::ProcessEvents()
    {
        sf::Event event;
        while (_mainViewport->_window.pollEvent(event))
        {
            _activeMode->ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                _mainViewport->_window.close();
        }
    }

    void Engine::InitWindow()
    {
        _mainViewport = CreateViewport<rendering::RenderViewport>();
        _mainViewport->_window.create(sf::VideoMode(1800, 1080), "ReEngine");
    }


}


