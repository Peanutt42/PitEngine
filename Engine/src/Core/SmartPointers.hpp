#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	template<class T>
	struct Ref {
		Ref(T* ptr = nullptr) : m_Ptr(ptr) {}

		Ref(const Ref& obj) = delete;
		Ref& operator=(const Ref& obj) = delete;

		Ref(Ref&& dyingObj) {
			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		~Ref() {
			_Cleanup();
			std::cout << "Raw pointer successfully deleted";
		}

		void operator=(Ref&& dyingObj) {
			_Cleanup();

			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		explicit operator bool() const { return m_Ptr; }

		T* operator->() {
			return m_Ptr;
		}

		T& operator*() {
			return *m_Ptr;
		}

	private:
		T* m_Ptr;

		inline void _Cleanup() {
			if (m_Ptr) delete m_Ptr;
		}
	};

	// NativeArray specific implomentation
	/*template<class T>
	struct Ref<T[]> {
		Ref(T* ptr = nullptr) : m_Ptr(ptr) {}

		Ref(const Ref& obj) = delete;
		Ref& operator=(const Ref& obj) = delete;

		Ref(Ref&& dyingObj) {
			_Cleanup();

			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		~Ref() {
			_Cleanup();
		}

		void operator=(Ref&& dyingObj) {
			_Cleanup();

			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		T* operator->() {
			return m_Ptr;
		}

		T& operator*() {
			return *m_Ptr;
		}

		T& operator[](int index) {
			if (index < 0)
				throw std::runtime_error("Negative index exception"));
			size_t arraySize = *(&m_Ptr + 1) - m_Ptr;
			if (index >= arraySize)
				throw std::runtime_error("Array index out of bounds!");
			
			return m_Ptr[index];
		}

	private:
		T* m_Ptr = nullptr;

		inline void _Cleanup() {
			if (m_Ptr) delete[] m_Ptr;
		}
	};*/
}