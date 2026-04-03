#include "hzpch.h"
#include "BreakoutLayer.h"

BreakoutLayer::BreakoutLayer(unsigned int width, unsigned int height)
    : Layer("Breakout"), m_Game(width, height),
    m_Camera(0.0f, (float)width, (float)height, 0.0f)
{
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

    // feed keys
    m_Game.Keys[LE_KEY_A] = LiteEngine::Input::IsKeyPressed(LE_KEY_A);
    m_Game.Keys[LE_KEY_D] = LiteEngine::Input::IsKeyPressed(LE_KEY_D);

    m_Game.ProcessInput(dt);
    m_Game.Update(dt);

    LiteEngine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
    LiteEngine::RenderCommand::Clear();

    LiteEngine::Renderer2D::BeginScene(m_Camera);
    m_Game.Render();
    LiteEngine::Renderer2D::EndScene();
}

void BreakoutLayer::OnEvent(LiteEngine::Event& e) {}