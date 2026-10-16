#pragma once
#include "LiteEngine.h"

class test1 : public LiteEngine::Layer
{
public:
    test1();
    void OnUpdate(LiteEngine::Timestep ts) override;
private:
    LiteEngine::OrthographicCamera m_Camera;
    glm::vec2 m_Position;
    float m_Speed = 300.0f;
};