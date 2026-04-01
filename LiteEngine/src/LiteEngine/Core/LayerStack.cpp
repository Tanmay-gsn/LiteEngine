#include "hzpch.h"
#include "LayerStack.h"

namespace LiteEngine {

	LayerStack::LayerStack()
	{
       m_LayerInsertIndex = 0;
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
     // compute iterator from index because stored iterator may be invalid
		auto it = m_Layers.begin() + m_LayerInsertIndex;
		auto newIt = m_Layers.emplace(it, layer);
		m_LayerInsertIndex = static_cast<size_t>(std::distance(m_Layers.begin(), newIt));
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
         size_t erasedIndex = static_cast<size_t>(std::distance(m_Layers.begin(), it));
			m_Layers.erase(it);
			if (m_LayerInsertIndex > 0 && erasedIndex <= m_LayerInsertIndex)
				m_LayerInsertIndex--;
			layer->OnDetach();
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			overlay->OnDetach();
		}
	}

}