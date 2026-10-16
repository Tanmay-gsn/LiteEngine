#include "hzpch.h"
#include "test1.h"

test1::test1()
    : Layer("Playground"),
    m_Camera(0.0f, 1280.0f, 720.0f, 0.0f),
    m_Position(600.0f, 335.0f)
{
}

void test1::OnUpdate(LiteEngine::Timestep ts)
{
    float dt = ts;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_A)) m_Position.x -= m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_D)) m_Position.x += m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_W)) m_Position.y -= m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_S)) m_Position.y += m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_UP)) m_Position.y -= m_Speed*dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_LEFT)) m_Position.x -= m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_RIGHT)) m_Position.x += m_Speed * dt;
    if (LiteEngine::Input::IsKeyPressed(LE_KEY_DOWN)) m_Position.y += m_Speed * dt;

    LiteEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f }); // RGBA
    LiteEngine::RenderCommand::Clear();

    LiteEngine::Renderer2D::BeginScene(m_Camera);

    LiteEngine::Renderer2D::DrawQuad(
        glm::vec3(m_Position, 0.0f),
        glm::vec2(50.0f, 150.0f),
        glm::vec4(0.0f, 0.0f, 0.6f, 1.0f)
    );

    LiteEngine::Renderer2D::EndScene();
}