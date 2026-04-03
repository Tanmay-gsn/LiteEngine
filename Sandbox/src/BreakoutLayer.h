#pragma once
#include "LiteEngine.h"
#include "Game.h"

class BreakoutLayer : public LiteEngine::Layer {
public:
    BreakoutLayer(unsigned int width, unsigned int height);
    ~BreakoutLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(LiteEngine::Timestep ts) override;
    void OnEvent(LiteEngine::Event& e) override;

private:
    Game m_Game;
    LiteEngine::OrthographicCamera m_Camera;
};