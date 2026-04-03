#pragma once
#include "LiteEngine.h"
#include "ResourceManager.h"

class GameObject {
public:
    glm::vec2                        Position, Size, Velocity;
    glm::vec3                        Color;
    float                            Rotation;
    bool                             IsSolid;
    bool                             Destroyed;
    LiteEngine::Ref<LiteEngine::Texture2D> Sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size,
        LiteEngine::Ref<LiteEngine::Texture2D> sprite,
        glm::vec3 color = glm::vec3(1.0f),
        glm::vec2 velocity = glm::vec2(0.0f));

    virtual void Draw();
};