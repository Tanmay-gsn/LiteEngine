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

    m_Game.Keys[LE_KEY_A] = LiteEngine::Input::IsKeyPressed(LE_KEY_A);
    m_Game.Keys[LE_KEY_D] = LiteEngine::Input::IsKeyPressed(LE_KEY_D);
    m_Game.Keys[LE_KEY_W] = LiteEngine::Input::IsKeyPressed(LE_KEY_W);
    m_Game.Keys[LE_KEY_S] = LiteEngine::Input::IsKeyPressed(LE_KEY_S);
    m_Game.Keys[LE_KEY_SPACE] = LiteEngine::Input::IsKeyPressed(LE_KEY_SPACE);
    m_Game.Keys[LE_KEY_ENTER] = LiteEngine::Input::IsKeyPressed(LE_KEY_ENTER);
    m_Game.Keys[LE_KEY_ESCAPE] = LiteEngine::Input::IsKeyPressed(LE_KEY_ESCAPE);

    if (!m_Game.Keys[LE_KEY_ENTER]) m_Game.KeysProcessed[LE_KEY_ENTER] = false;
    if (!m_Game.Keys[LE_KEY_W])     m_Game.KeysProcessed[LE_KEY_W] = false;
    if (!m_Game.Keys[LE_KEY_S])     m_Game.KeysProcessed[LE_KEY_S] = false;
    if (!m_Game.Keys[LE_KEY_ESCAPE]) m_Game.KeysProcessed[LE_KEY_ESCAPE] = false;

    m_Game.ProcessInput(dt);
    m_Game.Update(dt);

    LiteEngine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
    LiteEngine::RenderCommand::Clear();

    LiteEngine::Renderer2D::BeginScene(m_Camera);
    m_Game.Render();
    LiteEngine::Renderer2D::EndScene();
}

void BreakoutLayer::OnEvent(LiteEngine::Event& e) {}