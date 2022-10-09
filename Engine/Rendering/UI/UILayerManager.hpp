#pragma once

#include "Main/CoreInclude.hpp"
#include "UILayer.hpp"

namespace Pit::Rendering::UI {
	class LayerManager {
	public:
		LayerManager();
		~LayerManager();

		void RenderLayers();

		template<typename T>
		void PushLayer() {
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}
		void PushLayer(const std::shared_ptr<Layer>& layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(); }

		void RemoveLayer(const std::shared_ptr<Layer>& layer) {
			for (int i = 0; i < m_LayerStack.size(); i++) {
				if (m_LayerStack[i] == layer) { layer->OnDetach(); m_LayerStack.erase(m_LayerStack.begin() + i); }
			}
		}

	private:
		std::vector<std::shared_ptr<Layer>> m_LayerStack;
	};
}