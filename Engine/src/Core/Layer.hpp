#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	class Layer {
	public:
		Layer(const String& name) : m_Name(name) {}
		virtual ~Layer() = default;

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnUIRender() {}

		const String& GetName() { return m_Name; }

	private:
		String m_Name;
	};
}