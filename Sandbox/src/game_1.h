#pragma once
#include "LiteEngine.h"
#include "hzpch.h"

struct Object
{
	glm::vec2 position;
	glm::vec2 size;
	glm::vec4 color;
	LiteEngine::Ref<LiteEngine::Texture2D> texture = nullptr;
};
class game_1 : public LiteEngine::Layer
{
public:
	game_1();
	void OnUpdate(LiteEngine::Timestep ts) override;
	bool check_collision(Object obj1, Object obj2);
private:
	LiteEngine::Ref<LiteEngine::Texture2D> m_texture;
	LiteEngine::OrthographicCamera m_camera;
	std::vector<Object> m_objs;
	float m_speed = 500;
	int collision_dir = 0; // (-1,1,-2,2) -> (left,right,down,up)
	float timer = 0.0;
};