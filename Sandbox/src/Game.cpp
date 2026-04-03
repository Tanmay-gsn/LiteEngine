#include "hzpch.h"
#include "Game.h"
#include "ResourceManager.h"

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
}

Game::~Game() {}

void Game::Init()
{
    // load textures
    ResourceManager::LoadTexture("assets/textures/LE_logo.png", "face");
}

void Game::ProcessInput(float dt) {}

void Game::Update(float dt) {}

void Game::Render()
{
    // position (200,200), size (300,400), green tint
    LiteEngine::Renderer2D::DrawQuad(
        { 650.0f, 350.0f, 0.0f },
        { 600.0f, 600.0f },
        ResourceManager::GetTexture("face")
    );
}