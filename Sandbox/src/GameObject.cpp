#include "hzpch.h"
#include "GameObject.h"

GameObject::GameObject()
    : Position(0.0f), Size(1.0f), Velocity(0.0f), Color(1.0f),
    Rotation(0.0f), IsSolid(false), Destroyed(false), Sprite(nullptr) {
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size,
    LiteEngine::Ref<LiteEngine::Texture2D> sprite,
    glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color),
    Rotation(0.0f), IsSolid(false), Destroyed(false), Sprite(sprite) {
}

void GameObject::Draw()
{
    LiteEngine::Renderer2D::DrawQuad(
        { Position.x + Size.x / 2.0f, Position.y + Size.y / 2.0f, 0.0f },
        { Size.x, Size.y },
        Sprite
    );
}