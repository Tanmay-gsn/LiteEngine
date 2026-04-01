#pragma once

#include "LiteEngine/Core/Core.h"
#include "Layer.h"

#include <vector>

namespace LiteEngine {

	class LITEENGINE_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
   // store insertion point as an index because vector iterators
	// can be invalidated by reallocations (emplace_back, reserve, etc.)
	size_t m_LayerInsertIndex;
	};

}