#include "hzpch.h"
#include "BreakoutLayer.h"
#include "LiteEngine/Core/KeyCodes.h"

BreakoutLayer::BreakoutLayer(unsigned int width, unsigned int height)
    : Layer("Breakout"), m_Game(width, height) {
}

BreakoutLayer::~BreakoutLayer() {}

void BreakoutLayer::OnAttach()
{
    m_Game.Init();
}

void BreakoutLayer::OnDetach() {}

void BreakoutLayer::OnUpdate(LiteEngine::Timestep ts)
{
    float dt = ts;

    m_Game.ProcessInput(dt);
    m_Game.Update(dt);

    LiteEngine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
    LiteEngine::RenderCommand::Clear();
    m_Game.Render();
}

void BreakoutLayer::OnEvent(LiteEngine::Event& e) {}