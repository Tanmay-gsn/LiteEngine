#include "game_1.h"

game_1::game_1()
	:Layer("Game_1"),
	m_camera(0, 1280, 720, 0)
{
	m_texture = LiteEngine::Texture2D::Create("assets/textures/background.jpg");
	m_objs.push_back({
		{640,360},
		{1280,720},
		{0.1,0.1,0.5,1},
		m_texture
	});
	m_objs.push_back({
		{640,360},
		{100,100},
		{0.1,0.85,1,1}
	});
	m_objs.push_back({
		{1000,500},
		{200,50},
		{0.85,0.1,1,1}
	});
}
bool game_1::check_collision(Object obj1, Object obj2)
{
	float obj1Left = obj1.position.x - obj1.size.x / 2.0f;
	float obj1Right = obj1.position.x + obj1.size.x / 2.0f;
	float obj1Top = obj1.position.y - obj1.size.y / 2.0f;
	float obj1Bottom = obj1.position.y + obj1.size.y / 2.0f;

	float obj2Left = obj2.position.x - obj2.size.x / 2.0f;
	float obj2Right = obj2.position.x + obj2.size.x / 2.0f;
	float obj2Top = obj2.position.y - obj2.size.y / 2.0f;
	float obj2Bottom = obj2.position.y + obj2.size.y / 2.0f;

	if (obj1Right < obj2Left ||
		obj1Left > obj2Right ||
		obj1Bottom < obj2Top ||
		obj1Top > obj2Bottom)
	{
		return false;
	}

	float overlapX =
		std::min(obj1Right, obj2Right) -
		std::max(obj1Left, obj2Left);

	float overlapY =
		std::min(obj1Bottom, obj2Bottom) -
		std::max(obj1Top, obj2Top);

	if (overlapX < overlapY)
	{
		if (obj1.position.x < obj2.position.x)
			collision_dir = -1;
		else
			collision_dir = 1;
	}
	else
	{
		if (obj1.position.y < obj2.position.y)
			collision_dir = -2;
		else
			collision_dir = 2;
	}
	return true;
}

void game_1::OnUpdate(LiteEngine::Timestep ts)
{	
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_S)) m_objs[1].position.y += m_speed * ts;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_A)) m_objs[1].position.x -= m_speed * ts;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_W)) m_objs[1].position.y -= m_speed * ts;
	if (LiteEngine::Input::IsKeyPressed(LE_KEY_D)) m_objs[1].position.x += m_speed * ts;
	
	m_objs[1].position.x = glm::clamp(m_objs[1].position.x, 0.0f + m_objs[1].size.x / 2.0f, 1280.0f - m_objs[1].size.x/2.0f);
	m_objs[1].position.y = glm::clamp(m_objs[1].position.y, 0.0f + m_objs[1].size.y / 2.0f, 720.0f - m_objs[1].size.y/2.0f);

	if (timer <= 0.0f)
	{
		if (check_collision(m_objs[1], m_objs[2]))
		{
			timer = 0.2f;
		}
	}
	else
	{
		if (collision_dir == -1)
			m_objs[1].position.x -= 2.0f * m_speed * ts;
		else if (collision_dir == 1)
			m_objs[1].position.x += 2.0f * m_speed * ts;
		else if (collision_dir == -2)
			m_objs[1].position.y -= 2.0f * m_speed * ts;
		else if (collision_dir == 2)
			m_objs[1].position.y += 2.0f * m_speed * ts;

		timer -= ts;
	}

	LiteEngine::RenderCommand::SetClearColor({ 0.05,0.06,0.06,1.0 });
	LiteEngine::RenderCommand::Clear();

	LiteEngine::Renderer2D::BeginScene(m_camera);

	LiteEngine::Renderer2D::DrawQuad(m_objs[0].position, m_objs[0].size, m_objs[0].texture);
	LiteEngine::Renderer2D::DrawQuad(m_objs[1].position, m_objs[1].size, m_objs[1].color);
	LiteEngine::Renderer2D::DrawQuad(m_objs[2].position, m_objs[2].size, m_objs[2].color);

	LiteEngine::Renderer2D::EndScene();
}