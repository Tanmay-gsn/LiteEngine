#pragma once

#include "LiteEngine/Core/Core.h"
#include "LiteEngine/Core/Timestep.h"
#include "LiteEngine/Events/Event.h"

namespace LiteEngine {

	class LITEENGINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}