#pragma once

#include <vector>
#include <functional>

namespace Pit {
	template<typename... Args>
	struct Event {
		using ListenFunc = std::function<void(Args...)>;

		inline void operator+=(ListenFunc func) {
			Add(func);
		}

		inline void operator-=(ListenFunc func) {
			Remove(func);
		}

		inline void operator()(Args&&... args) {
			Invoke(std::forward<Args>(args)...);
		}

		inline void Add(ListenFunc func) {
			m_Listeners.push_back(func);
		}

		inline void Remove(ListenFunc func) {
			for (int i = 0; i < m_Listeners.size(); i++) {
				if (m_Listeners[i] == func)
					m_Listeners.erase(m_Listeners.begin() + i);
			}
		}

		inline void Invoke(Args&&... args) {
			for (auto& callack : m_Listeners)
				callack(std::forward<Args>(args)...);
		}

		inline void Reset() {
			m_Listeners.clear();
		}

		inline explicit operator bool() {
			return !m_Listeners.empty();
		}

	private:
		std::vector<ListenFunc> m_Listeners;
	};
}