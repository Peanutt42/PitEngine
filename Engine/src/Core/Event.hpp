#pragma once

#include <vector>
#include <functional>
#include "Debug/Profiling.hpp"

namespace Pit {
	/// <summary>
	/// Simple Wrapper around std::function(of void(Args...))[] with operator overloading
	/// </summary>
	/// <typeparam name="EventArgs"></typeparam>
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
			m_Listeners.emplace_back(func);
		}

		inline void Remove(ListenFunc func) {
			for (int i = 0; i < m_Listeners.size(); i++) {
				if (m_Listeners[i] == func)
					m_Listeners.erase(m_Listeners.begin() + i);
			}
		}

		inline virtual void Invoke(Args... args) {
			PIT_PROFILE_FUNCTION("Pit::Event<Args...>::Invoke");

			for (auto& callack : m_Listeners)
				callack(std::forward<Args>(args)...);
		}

		inline void Reset() {
			m_Listeners.clear();
		}

		inline explicit operator bool() {
			return !m_Listeners.empty();
		}

	protected:
		std::vector<ListenFunc> m_Listeners;
	};

	template<>
	inline virtual void Event<>::Invoke() {
		PIT_PROFILE_FUNCTION("Pit::Event::Invoke");

		for (auto& callback : m_Listeners)
			callback();
	}

#define DEFINE_SIMPLE_EVENT(name) \
	class name : public Event<> {									\
	public:															\
		inline virtual void Invoke() override {						\
			PIT_PROFILE_FUNCTION("Pit::" #name "::Invoke");			\
																	\
			for (auto& callack : m_Listeners)						\
				callack();											\
		}															\
	}
}
