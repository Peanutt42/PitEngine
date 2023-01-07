#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSScene.hpp"

namespace Pit::ECS {
#define ECS_NULL_CHECKS (DEBUG)

	/// <summary>
	/// Handle for the native entt::entity id that also
	/// has a reference to the EcsWorld, so it's more OOP
	/// like Entity.AddComponent(component);
	/// </summary>
	class EntityHandle {
	public:
		EntityHandle(Scene* scene, entt::entity id) : m_Scene(scene), m_ID(id) {}

		inline void Destroy() {
			m_Scene->DestroyEntity(m_ID);
		}

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) {
#if ECS_NULL_CHECKS
			if (m_Scene->HasComponent<T>(m_ID)) {
				PIT_ENGINE_ERR(ECS, "Entity already has this type of component!");
				return m_Scene->GetComponent<T>(m_ID);
			}
#endif
			return m_Scene->AddComponent<T>(m_ID, std::forward<Args>(args)...);
		}

		template<typename T>
		inline T& GetComponent() {
#if ECS_NULL_CHECKS
			if (!m_Scene->HasComponent<T>(m_ID))
				PIT_ENGINE_FATAL(ECS, "Entity doesn't have this type of component!");
#endif
			return m_Scene->GetComponent<T>(m_ID);
		}

		template<typename T>
		inline bool HasComponent() {
			return m_Scene->HasComponent<T>(m_ID);
		}

		template<typename T>
		inline T* TryGetComponent() {
			T* component = nullptr;
			m_Scene->TryGetComponent<T>(m_ID, component);
			return component;
		}

		template<typename T>
		inline void ReplaceComponent(T& newComponent) {
			m_Scene->ReplaceComponent<T>(m_ID);
		}

		template<typename T>
		inline T& AddOrReplaceComponent(T& component) {
			m_Scene->AddOrReplaceComponent(m_ID, component);
		}

		template<typename T, typename... Args>
		inline T& AddOrGetComponent(T& component, Args&&... args) {
			if (m_Scene->HasComponent<T>())
				return m_Scene->GetComponent<T>(m_ID);
			else
				return m_Scene->AddComponent<T>(m_ID, std::forward<Args>(args)...);
		}

		template<typename T>
		inline void RemoveComponent() {
#if ECS_NULL_CHECKS
			if (!HasComponent<T>()) {
				PIT_ENGINE_ERR(ECS, "Can't remove a component of entity that doesn't has one of them!");
				return;
			}
#endif
			m_Scene->RemoveComponent<T>(m_ID);
		}

		operator bool() { return m_Scene && m_Scene->GetRegistry().valid(m_ID); }

		entt::entity GetID() { return m_ID; }
		Scene* GetScene() { return m_Scene; }

	private:
		Scene* m_Scene;
		entt::entity m_ID;
	};
}