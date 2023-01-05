#pragma once

namespace Pit {
	/// <summary>
	/// SmartPointer that gets automatically destroyed when out of scope
	/// </summary>
	template<class T>
	class ScopeRef {
	public:
		ScopeRef(T* ptr = nullptr) : m_Ptr(ptr) {}

		ScopeRef(const ScopeRef<T>& obj) = delete;
		ScopeRef& operator=(const ScopeRef<T>& obj) = delete;

		ScopeRef(ScopeRef<T>&& dyingObj) noexcept {
			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		~ScopeRef() {
			_Cleanup();
		}

		void operator=(ScopeRef<T>&& dyingObj) noexcept {
			_Cleanup();

			m_Ptr = dyingObj.m_Ptr;
			dyingObj.m_Ptr = nullptr;
		}

		explicit operator const bool() const { return m_Ptr; }

		T* operator->() {
			#if DEBUG
			if (!m_Ptr) std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return m_Ptr;
		}

		T& operator*() {
			#if DEBUG
			if (!m_Ptr)	std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return *m_Ptr;
		}

		T* Get() {
			#if DEBUG
			if (!m_Ptr)	std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return m_Ptr;
		}

		void Release() {
			_Cleanup();
		}

	private:
		T* m_Ptr;

		inline void _Cleanup() {
			#if DEBUG
			if (m_Ptr) {
			#endif
				delete m_Ptr;
				m_Ptr = nullptr;
			#if DEBUG
			}
			#endif
		}
	};

	template<typename T, typename... Args>
	static __forceinline ScopeRef<T> CreateScopeRef(Args&&... args) {
		return ScopeRef<T>(new T(std::forward<Args>(args)...));
	}


	struct ReferenceCounter {
		ReferenceCounter(unsigned short startingCount) : m_Count(startingCount) {}

		void AddRef() { m_Count++; }
		unsigned short Release() { return --m_Count; }
		unsigned short GetCount() { return m_Count; }

	private:
		unsigned short m_Count;
	};

	/// <summary>
	/// Smartpointer with automatic deletion, including reference counting
	/// </summary>
	template<class T>
	class Ref {
	public:
		Ref(T* ptr = nullptr) : m_Ptr(ptr) {
			
			m_References = new ReferenceCounter(1);
		}

		Ref(const Ref<T>& obj) : m_Ptr(obj.m_Ptr), m_References(obj.m_References) {
			obj.m_References->AddRef();
		}

		~Ref() {
			if (m_References->Release() == 0)
				_Cleanup();
		}

		Ref<T>& operator=(const Ref<T>& other) {
			if (this != &other) {
				if (m_References->Release() == 0)
					_Cleanup();

				m_Ptr = other.m_Ptr;
				m_References = other.m_References;
				m_References->AddRef();
			}
			return *this;
		}

		explicit operator const bool() const { return m_Ptr && m_References; }

		T* operator->() {
			#if DEBUG
			if (!m_Ptr) std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return m_Ptr;
		}

		T& operator*() {
			#if DEBUG
			if (!m_Ptr) std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return *m_Ptr;
		}

		T* Get() {
			#if DEBUG
			if (!m_Ptr) std::cerr << "[Engine::General]    Pointer was accessed but was nullptr!\n";
			#endif
			return m_Ptr;
		}

		void Release() {
			_Cleanup();
		}

		const unsigned short GetReferences() const {
			if (m_References) return m_References->GetCount();
			
			return 0;
		}


	private:
		T* m_Ptr;
		ReferenceCounter* m_References;

		inline void _Cleanup() {
			#if DEBUG
			if (m_Ptr) {
			#endif
				delete m_Ptr;
				m_Ptr = nullptr;
			#if DEBUG
			}
			#endif
			#if DEBUG
			if (m_References) {
			#endif
				delete m_References;
				m_References = nullptr;
			#if DEBUG	
			}
			#endif
		}
	};

	template<typename T, typename... Args>
	static __forceinline Ref<T> CreateRef(Args&&... args) {
		return Ref<T>(new T(std::forward<Args>(args)...));
	}
}