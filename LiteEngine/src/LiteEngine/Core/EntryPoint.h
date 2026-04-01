#pragma once

#ifdef LE_PLATFORM_WINDOWS

extern LiteEngine::Application* LiteEngine::CreateApplication();

int main(int argc, char** argv)
{
	LiteEngine::Log::Init();
	LE_CORE_WARN("Initialized Log!");
	int a = 7;
	LE_INFO("Hello! var={0}",a);

	auto app = LiteEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif