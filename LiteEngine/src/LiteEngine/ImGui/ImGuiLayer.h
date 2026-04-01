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
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}