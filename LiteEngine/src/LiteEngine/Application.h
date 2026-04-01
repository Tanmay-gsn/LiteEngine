#pragma once

#include "Core.h"

#include "Windows.h"
#include "LiteEngine/LayerStack.h"
#include "LiteEngine/Events/Event.h"
#include "LiteEngine/Events/ApplicationEvent.h"

#include "LiteEngine/Core/Timestep.h"

#include "LiteEngine/ImGui/ImGuiLayer.h"

struct ImGuiContext;

namespace LiteEngine {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
		ImGuiContext* GetImGuiContext();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}