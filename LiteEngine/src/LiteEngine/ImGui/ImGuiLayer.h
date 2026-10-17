#pragma once

#include "LiteEngine/Core/Layer.h"

#include "LiteEngine/Events/ApplicationEvent.h"
#include "LiteEngine/Events/KeyEvent.h"
#include "LiteEngine/Events/MouseEvent.h"

namespace LiteEngine {

	class LITEENGINE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};

}