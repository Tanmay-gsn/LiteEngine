#include "hzpch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "LiteEngine/Core/KeyCodes.h"

GameObject* Player;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Level(0) {
}

Game::~Game() {}

void Game::Init()
{
    // load textures
    ResourceManager::LoadTexture("assets/textures/background.jpg", "background");
    ResourceManager::LoadTexture("assets/textures/block.png", "block");
    ResourceManager::LoadTexture("assets/textures/block_solid.png", "block_solid");
    ResourceManager::LoadTexture("assets/textures/paddle.png", "paddle");

    // load levels
    GameLevel one;   one.Load("assets/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two;   two.Load("assets/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("assets/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four;  four.Load("assets/levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
    LE_CORE_INFO("Level 0 brick count: {0}", this->Levels[0].Bricks.size()); // add this


    // init player
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[LE_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
                Player->Position.x -= velocity;
        }
        if (this->Keys[LE_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
    }
}

void Game::Update(float dt) {}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // background
        LiteEngine::Renderer2D::DrawQuad(
            { this->Width / 2.0f, this->Height / 2.0f, 0.0f },
            { (float)this->Width, (float)this->Height },
            ResourceManager::GetTexture("background")
        );
        // level
        this->Levels[this->Level].Draw();
        // player
        Player->Draw();
    }
}