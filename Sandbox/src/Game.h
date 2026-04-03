#pragma once
#include "LiteEngine.h"
#include "ResourceManager.h"
#include "GameLevel.h"
#include "GameObject.h"

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    GameState                State;
    bool                     Keys[1024];
    unsigned int             Width, Height;
    std::vector<GameLevel>   Levels;
    unsigned int             Level;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};