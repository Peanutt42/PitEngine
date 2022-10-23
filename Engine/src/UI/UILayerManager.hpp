#pragma once

#include "Core/CoreInclude.hpp"
#include "UILayer.hpp"

namespace Pit::UI {
	class LayerManager {
	public:
		LayerManager();
		~LayerManager();

		void RenderLayers();

		void SetCallbacks(std::function<void()> beginCallback, std::function<void()> endCallback) {
			m_OnBeginCallback = beginCallback;
			m_OnEndCallback = endCallback;
		}

		template<typename T>
		void PushLayer() {
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			T* newLayer = new T();
			m_LayerStack.emplace_back(newLayer);
			newLayer->OnAttach();
		}
		void PushLayer(Layer* layer) {
			m_LayerStack.emplace_back(layer);
			layer->OnAttach();
		}

		void RemoveLayer(Layer& layer) {
			for (int i = 0; i < m_LayerStack.size(); i++) {
				if (m_LayerStack[i] == &layer) { layer.OnDetach(); m_LayerStack.erase(m_LayerStack.begin() + i); }
			}
		}

	private:
		std::vector<Layer*> m_LayerStack;
		std::function<void()> m_OnBeginCallback;
		std::function<void()> m_OnEndCallback;
	};
}