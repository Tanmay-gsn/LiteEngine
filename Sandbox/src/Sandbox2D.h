#pragma once
#include "LiteEngine.h"

class Sandbox2D : public LiteEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(LiteEngine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(LiteEngine::Event& e) override;

private:
	LiteEngine::OrthographicCameraController m_CameraController;

	// Temp
	LiteEngine::Ref<LiteEngine::VertexArray> m_SquareVA;
	LiteEngine::Ref<LiteEngine::Shader> m_FlatColorShader;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};