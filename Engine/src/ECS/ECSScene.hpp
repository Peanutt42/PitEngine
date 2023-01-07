#pragma once

#include "Core/CoreInclude.hpp"
#include <entt.hpp>
#include "Rendering/Camera.hpp"

namespace Pit::ECS {
	class EntityHandle;
	class Scene;

	/// <summary>
	/// ECSScene
	/// </summary>
	class Scene {
	public:
		Scene(const String& name);

		bool Init();
		void UpdateRuntime();
		void UpdateEditor();
		void Clear();

#pragma region Entity
		entt::entity CreateEntityID();
		EntityHandle CreateEntity();
		void DestroyEntity(entt::entity entity);
#pragma endregion

#pragma region Component
		template<typename T>
		const bool HasComponent(entt::entity entity) const {
			return m_Registry.any_of<T>(entity);
		}

		template<typename T, typename... Args>
		T& AddComponent(entt::entity entity, Args&&... args) {
			#if DEBUG
			m_ComponentMemSize += sizeof(T);
			m_ComponentCount++;
			#endif
			return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent(entt::entity entity) {
			return m_Registry.get<T>(entity);
		}

		template<typename T>
		bool TryGetComponent(entt::entity entity, T* OutComponent) {
			OutComponent = m_Registry.try_get<T>(entity);
			return OutComponent != nullptr;
		}

		template<typename T>
		void RemoveComponent(entt::entity entity) {
			#if DEBUG
			m_ComponentMemSize -= sizeof(T);
			m_ComponentCount--;
			#endif
			m_Registry.remove<T>(entity);
		}

		template<typename T>
		bool TryRemoveComponent(entt::entity entity) {
			if (HasComponent<T>(entity)) {
				#if DEBUG
				m_ComponentMemSize -= sizeof(T);
				m_ComponentCount--;
				#endif
				m_Registry.remove<T>(entity);
				return true;
			}
			return false;
		}

		template<typename T>
		void ReplaceComponent(entt::entity entity, T& newComponent) {
			if (!HasComponent<T>(entity)) return;

			m_Registry.replace(entity, newComponent);
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(entt::entity entity, Args&&... args) {
			#if DEBUG
			if (!HasComponent<T>(entity)) {
				m_ComponentMemSize += sizeof(T);
				m_ComponentCount++;
			}
			#endif
			return m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& GetOrAddComponent(entt::entity entity, Args&&... args) {
			#if DEBUG
			if (!HasComponent<T>(entity)) m_ComponentMemSize += sizeof(T);
			#endif
			return m_Registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
		}
#pragma endregion

		template<typename... T>
		auto View() {
			return m_Registry.view<T...>();
		}

		template<typename... T>
		auto Group() {
			return m_Registry.group<T...>();
		}

		template<typename Func>
		void Each(Func func) {
			m_Registry.each(func);
		}

		const String& GetName() const;
		void SetName(const String& name);

		Rendering::Camera& GetCamera() { return m_Camera; }
		void SetCamera(Rendering::Camera& newCamera) { m_Camera = newCamera; }
		
		entt::registry& GetRegistry() { return m_Registry; }

	private:
		String m_Name;
		entt::registry m_Registry;

		Rendering::Camera m_Camera;

		#if DEBUG
		size m_ComponentMemSize = 0;
		size m_ComponentCount = 0;
		#endif

		friend EntityHandle;
	};
}
