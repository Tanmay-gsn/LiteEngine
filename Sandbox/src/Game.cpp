#include "hzpch.h"
#include "game.h"

Game::Game()
	: Layer("Game"),
	  m_Camera(0.0f, 1280.0f, 720.0f, 0.0f),
	  m_Position(640.0f, 360.0f),
	  m_Size(80.0f, 80.0f),
	  m_Color(0.2f, 0.8f, 1.0f, 1.0f),
	  m_Speed(400.0f)
{
}

void Game::OnUpdate(LiteEngine::Timestep ts)
{
	float dt = std::min((float)ts, 0.05f);

	if (LiteEngine::Input::IsKeyPressed(LE_KEY_A)) m_Position.x -= m_Speed * dt;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_D)) m_Position.x += m_Speed * dt;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_W)) m_Position.y -= m_Speed * dt;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_S)) m_Position.y += m_Speed * dt;

	LiteEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f });
	LiteEngine::RenderCommand::Clear();

	LiteEngine::Renderer2D::BeginScene(m_Camera);
	LiteEngine::Renderer2D::DrawQuad(m_Position, m_Size, m_Color);
	LiteEngine::Renderer2D::EndScene();
}