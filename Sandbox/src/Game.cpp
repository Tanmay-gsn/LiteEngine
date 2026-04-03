#include "hzpch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "LiteEngine/Core/KeyCodes.h"

GameObject* Player;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Level(0) {
}

Game::~Game() {}

void Game::Init()
{
    ResourceManager::LoadTexture("assets/textures/background.jpg", "background");
    ResourceManager::LoadTexture("assets/textures/block.png", "block");
    ResourceManager::LoadTexture("assets/textures/block_solid.png", "block_solid");
    ResourceManager::LoadTexture("assets/textures/paddle.png", "paddle");
    ResourceManager::LoadTexture("assets/textures/ball.png", "ball");

    GameLevel one;   one.Load("assets/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two;   two.Load("assets/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("assets/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four;  four.Load("assets/levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(
        PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
        -BALL_RADIUS * 2.0f
    );
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture("ball"));
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[LE_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[LE_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[LE_KEY_SPACE])
            Ball->Stuck = false;
    }
}
bool CheckCollision(GameObject& one, GameObject& two)
{
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY;
}

bool CheckCollision(BallObject& one, GameObject& two)
{
    glm::vec2 center(one.Position + one.Radius);
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;
    return glm::length(difference) < one.Radius;
}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            if (CheckCollision(*Ball, box))
            {
                if (!box.IsSolid)
                    box.Destroyed = true;
            }
        }
    }
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        LiteEngine::Renderer2D::DrawQuad(
            { this->Width / 2.0f, this->Height / 2.0f, 0.0f },
            { (float)this->Width, (float)this->Height },
            ResourceManager::GetTexture("background")
        );
        this->Levels[this->Level].Draw();
        Player->Draw();
        Ball->Draw();
    }
}