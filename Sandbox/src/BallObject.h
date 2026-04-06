#pragma once
#include "GameObject.h"

class BallObject : public GameObject {
public:
    float Radius;
    bool  Stuck;
    bool  Sticky;       // Ball sticks to paddle on contact (power-up)
    bool  PassThrough;  // Ball passes through non-solid bricks (power-up)

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity,
        LiteEngine::Ref<LiteEngine::Texture2D> sprite);

    glm::vec2 Move(float dt, unsigned int window_width);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};