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

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f,  1.0f),
        glm::vec2(1.0f,  0.0f),
        glm::vec2(0.0f, -1.0f),
        glm::vec2(-1.0f, 0.0f)
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

bool CheckCollision(GameObject& one, GameObject& two)
{
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two)
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
    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision))
            {
                if (!box.IsSolid)
                    box.Destroyed = true;
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT)
                {
                    Ball->Velocity.x = -Ball->Velocity.x;
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration;
                    else
                        Ball->Position.x -= penetration;
                }
                else
                {
                    Ball->Velocity.y = -Ball->Velocity.y;
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration;
                    else
                        Ball->Position.y += penetration;
                }
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("assets/levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("assets/levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("assets/levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("assets/levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Ball->Reset(
        Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f),
        INITIAL_BALL_VELOCITY
    );
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
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