#pragma once
#include "LiteEngine.h"

class Game : public LiteEngine::Layer
{
public:
	Game();

	void OnUpdate(LiteEngine::Timestep ts) override;

private:
	LiteEngine::OrthographicCamera m_Camera;
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	glm::vec4 m_Color;
	float m_Speed;
};