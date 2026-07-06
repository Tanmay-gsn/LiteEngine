#include "hzpch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "AudioManager.h"
#include "LiteEngine/Core/KeyCodes.h"
#include <cstdlib>

GameObject* Player;
BallObject* Ball;

static bool ShouldSpawn(unsigned int chance)
{
    return (rand() % chance) == 0;
}

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU),
    Keys(), KeysProcessed(),
    Width(width), Height(height),
    Level(0), Lives(3),
    m_Text(nullptr)
{
}

Game::~Game()
{
    AudioManager::Shutdown();
    delete m_Text;
}

void Game::Init()
{
    ResourceManager::LoadTexture("assets/textures/menu_background.jpg", "menu_background");
    ResourceManager::LoadTexture("assets/textures/background.jpg", "background");
    ResourceManager::LoadTexture("assets/textures/block.png", "block");
    ResourceManager::LoadTexture("assets/textures/block_solid.png", "block_solid");
    ResourceManager::LoadTexture("assets/textures/paddle.png", "paddle");
    ResourceManager::LoadTexture("assets/textures/ball.png", "ball");
    ResourceManager::LoadTexture("assets/textures/powerup_speed.png", "powerup_speed");
    ResourceManager::LoadTexture("assets/textures/powerup_sticky.png", "powerup_sticky");
    ResourceManager::LoadTexture("assets/textures/powerup_passthrough.png", "powerup_passthrough");
    ResourceManager::LoadTexture("assets/textures/powerup_increase.png", "powerup_increase");
    ResourceManager::LoadTexture("assets/textures/powerup_confuse.png", "powerup_confuse");
    ResourceManager::LoadTexture("assets/textures/powerup_chaos.png", "powerup_chaos");

    AudioManager::Init();
    AudioManager::LoadSound("assets/audio/breakout.mp3", "music");
    AudioManager::LoadSound("assets/audio/bleep.mp3", "ball_paddle");
    AudioManager::LoadSound("assets/audio/bleep.wav", "ball_brick");
    AudioManager::LoadSound("assets/audio/solid.wav", "ball_solid");
    AudioManager::LoadSound("assets/audio/powerup.wav", "powerup");
    AudioManager::SetVolume("music", 0.4f);
    AudioManager::PlaySound("music", true);

    GameLevel one;   one.Load("assets/levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two;   two.Load("assets/levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("assets/levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four;  four.Load("assets/levels/four.lvl", this->Width, this->Height / 2);
    GameLevel five;  five.Load("assets/levels/five.lvl", this->Width, this->Height / 2);
    GameLevel six;   six.Load("assets/levels/six.lvl", this->Width, this->Height / 2);
    GameLevel seven; seven.Load("assets/levels/seven.lvl", this->Width, this->Height / 2);
    GameLevel eight; eight.Load("assets/levels/eight.lvl", this->Width, this->Height / 2);

    this->Levels.clear();
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Levels.push_back(five);
    this->Levels.push_back(six);
    this->Levels.push_back(seven);
    this->Levels.push_back(eight);
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

    m_Text = new TextRenderer(this->Width, this->Height);
    m_Text->Load("assets/fonts/ocraext.ttf", 24);
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[LE_KEY_ENTER] && !this->KeysProcessed[LE_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[LE_KEY_ENTER] = true;
        }
        if (this->Keys[LE_KEY_W] && !this->KeysProcessed[LE_KEY_W])
        {
            this->Level = (this->Level + 1) % this->Levels.size();
            this->KeysProcessed[LE_KEY_W] = true;
        }
        if (this->Keys[LE_KEY_S] && !this->KeysProcessed[LE_KEY_S])
        {
            this->Level = (this->Level > 0)
                ? this->Level - 1
                : (unsigned int)(this->Levels.size() - 1);
            this->KeysProcessed[LE_KEY_S] = true;
        }
    }

    if (this->State == GAME_WIN)
    {
        if (this->Keys[LE_KEY_ENTER] && !this->KeysProcessed[LE_KEY_ENTER])
        {
            this->KeysProcessed[LE_KEY_ENTER] = true;
            this->ResetLevel();
            this->ResetPlayer();
            this->Lives = 3;
            this->State = GAME_MENU;
        }
    }

    if (this->State == GAME_ACTIVE)
    {
        if (this->Keys[LE_KEY_ESCAPE] && !this->KeysProcessed[LE_KEY_ESCAPE])
        {
            this->KeysProcessed[LE_KEY_ESCAPE] = true;
            this->ResetLevel();
            this->ResetPlayer();
            this->Lives = 3;
            this->State = GAME_MENU;
        }

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
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                    AudioManager::PlaySound("ball_brick");
                }
                else
                {
                    AudioManager::PlaySound("ball_solid");
                }

                if (Ball->PassThrough && !box.IsSolid)
                    continue;

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
        Ball->Stuck = Ball->Sticky;
        AudioManager::PlaySound("ball_paddle");
    }

    for (PowerUp& powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = true;

            if (CheckCollision(*Player, powerUp))
            {
                this->ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
                powerUp.Activated = true;
                AudioManager::PlaySound("powerup");
            }
        }
    }
}

void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(40))
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 10.0f,
                block.Position, ResourceManager::GetTexture("powerup_speed")));

    if (ShouldSpawn(40))
        this->PowerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
                block.Position, ResourceManager::GetTexture("powerup_sticky")));

    if (ShouldSpawn(20))
        this->PowerUps.push_back(
            PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
                block.Position, ResourceManager::GetTexture("powerup_passthrough")));

    if (ShouldSpawn(400))
        this->PowerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 10.0f,
                block.Position, ResourceManager::GetTexture("powerup_increase")));

    if (ShouldSpawn(15))
        this->PowerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f,
                block.Position, ResourceManager::GetTexture("powerup_confuse")));

    if (ShouldSpawn(15))
        this->PowerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f,
                block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2f;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50.0f;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
            Ball->Velocity.x = -Ball->Velocity.x;
    }
    else if (powerUp.Type == "chaos")
    {
        if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
        {
            Ball->Velocity.x *= -1.0f;
            Ball->Velocity.y *= -1.0f;
        }
    }
}

bool Game::IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& p : powerUps)
        if (p.Activated && p.Type == type)
            return true;
    return false;
}

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;

        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                powerUp.Activated = false;

                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {
                        Ball->Sticky = false;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "speed")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "speed"))
                        Ball->Velocity /= 1.2f;
                }
                else if (powerUp.Type == "pad-size-increase")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pad-size-increase"))
                        Player->Size.x -= 50.0f;
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                        Ball->Velocity.x = -Ball->Velocity.x;
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {
                        Ball->Velocity.x *= -1.0f;
                        Ball->Velocity.y *= -1.0f;
                    }
                }
            }
        }
    }

    this->PowerUps.erase(
        std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
            [](const PowerUp& p) { return p.Destroyed && !p.Activated; }),
        this->PowerUps.end()
    );
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    this->UpdatePowerUps(dt);

    if (Ball->Position.y >= this->Height)
    {
        if (--this->Lives == 0)
        {
            this->ResetLevel();
            this->Lives = 3;
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }

    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetLevel();
        this->ResetPlayer();
        this->State = GAME_WIN;
    }
}

void Game::Render()
{
    std::string currentBackground = (this->State == GAME_MENU) ? "menu_background" : "background";

    LiteEngine::Renderer2D::DrawQuad(
        { this->Width / 2.0f, this->Height / 2.0f, 0.0f },
        { (float)this->Width, (float)this->Height },
        ResourceManager::GetTexture(currentBackground)
    );

    if (this->State == GAME_ACTIVE || this->State == GAME_WIN)
    {
        this->Levels[this->Level].Draw();
        Player->Draw();
        Ball->Draw();

        for (PowerUp& powerUp : this->PowerUps)
            if (!powerUp.Destroyed)
                powerUp.Draw();
    }

    if (this->State == GAME_ACTIVE)
    {
        m_Text->RenderText(
            "Lives: " + std::to_string(this->Lives),
            5.0f, 5.0f, 1.0f,
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        m_Text->RenderText(
            "Level: " + std::to_string(this->Level + 1),
            this->Width - 130.0f, 5.0f, 1.0f,
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
    }

    if (this->State == GAME_MENU)
    {
        m_Text->RenderText(
            "BREAKOUT",
            this->Width / 2.0f - 90.0f,
            this->Height / 2.0f - 80.0f,
            1.5f,
            glm::vec3(0.2f, 0.8f, 1.0f)
        );
        m_Text->RenderText(
            "Press ENTER to start",
            this->Width / 2.0f - 175.0f,
            this->Height / 2.0f - 20.0f,
            1.0f
        );
        m_Text->RenderText(
            "Press W / S to select level: " + std::to_string(this->Level + 1),
            this->Width / 2.0f - 260.0f,
            this->Height / 2.0f + 20.0f,
            0.75f,
            glm::vec3(0.8f, 0.8f, 0.8f)
        );
    }

    if (this->State == GAME_WIN)
    {
        m_Text->RenderText(
            "YOU WON!!!",
            this->Width / 2.0f - 100.0f,
            this->Height / 2.0f - 40.0f,
            1.5f,
            glm::vec3(0.0f, 1.0f, 0.5f)
        );
        m_Text->RenderText(
            "Press ENTER to play again",
            this->Width / 2.0f - 215.0f,
            this->Height / 2.0f + 20.0f,
            1.0f,
            glm::vec3(1.0f, 1.0f, 0.0f)
        );
    }
}
void Game::ResetLevel()
{
    if (this->Level == 0) this->Levels[0].Load("assets/levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1) this->Levels[1].Load("assets/levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2) this->Levels[2].Load("assets/levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3) this->Levels[3].Load("assets/levels/four.lvl", this->Width, this->Height / 2);
    else if (this->Level == 4) this->Levels[4].Load("assets/levels/five.lvl", this->Width, this->Height / 2);
    else if (this->Level == 5) this->Levels[5].Load("assets/levels/six.lvl", this->Width, this->Height / 2);
    else if (this->Level == 6) this->Levels[6].Load("assets/levels/seven.lvl", this->Width, this->Height / 2);
    else if (this->Level == 7) this->Levels[7].Load("assets/levels/eight.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Color = glm::vec3(1.0f);
    Player->Position = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Ball->Reset(
        Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f),
        INITIAL_BALL_VELOCITY
    );
    this->PowerUps.clear();
}