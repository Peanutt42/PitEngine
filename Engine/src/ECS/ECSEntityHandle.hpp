#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSWorld.hpp"

namespace Pit::ECS {
#define ECS_NULL_CHECKS (DEBUG || RELEASE)

	class EntityHandle {
	public:
		EntityHandle(World* world, entt::entity id) : m_World(world), m_ID(id) {}

		inline void Destroy() {
			m_World->DestroyEntity(m_ID);
		}

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) {
#if ECS_NULL_CHECKS
			if (m_World->HasComponent<T>(m_ID)) {
				PIT_ENGINE_ERR(Log::ECS, "Entity already has this type of component!");
				return m_World->GetComponent<T>(m_ID);
			}
#endif
			return m_World->AddComponent<T>(m_ID, std::forward<Args>(args)...);
		}

		template<typename T>
		inline T& GetComponent() {
#if ECS_NULL_CHECKS
			if (!m_World->HasComponent<T>(m_ID))
				PIT_ENGINE_FATAL(Log::ECS, "Entity doesn't have this type of component!");
#endif
			return m_World->GetComponent<T>(m_ID);
		}

		template<typename T>
		inline bool HasComponent() {
			return m_World->HasComponent<T>(m_ID);
		}

		template<typename T>
		inline T* TryGetComponent() {
			T* component = nullptr;
			m_World->TryGetComponent<T>(m_ID, component);
			return component;
		}

		template<typename T>
		inline void ReplaceComponent(T& newComponent) {
			m_World->ReplaceComponent<T>(m_ID);
		}

		template<typename T>
		inline T& AddOrReplaceComponent(T& component) {
			m_World->AddOrReplaceComponent(m_ID, component);
		}

		template<typename T, typename... Args>
		inline T& AddOrGetComponent(T& component, Args&&... args) {
			if (m_World->HasComponent<T>())
				return m_World->GetComponent<T>(m_ID);
			else
				return m_World->AddComponent<T>(m_ID, std::forward<Args>(args)...);
		}

		template<typename T>
		inline void RemoveComponent() {
#if ECS_NULL_CHECKS
			if (!HasComponent<T>()) {
				PIT_ENGINE_ERR(Log::ECS, "Can't remove a component of entity that doesn't has one of them!");
				return;
			}
#endif
			m_World->RemoveComponent<T>(m_ID);
		}

		inline entt::entity GetID() { return m_ID; }

	private:
		World* m_World;
		entt::entity m_ID;
	};
}