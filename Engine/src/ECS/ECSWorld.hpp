#pragma once

#include "Core/CoreInclude.hpp"
#include "ECSInclude.hpp"
#include "ECSSystem.hpp"


namespace Pit::ECS {
	struct EntityHandle;
	struct World;

	struct SystemHandle {
		SystemIndex SystemIndex;
		SystemTopic Topic = SystemTopic::None;
		int32 ExecuteOrder;
		std::function<void(World&)> Update;
	};
	
	struct WorldSpecs {
		String name = "ECS-World";

		WorldSpecs(const String& name) :
			name(name) { }
	};

	struct World {
	public:
		World(const WorldSpecs& specs) :
			m_Specs(specs) { }

		bool Init();
		void Update(const SystemTopic topic);
		void Clear();

#pragma region Entity
		entt::entity CreateEntityID();
		EntityHandle CreateEntity();
		void DestroyEntity(entt::entity entity);
#pragma endregion

#pragma region Component
		template<typename T>
		bool HasComponent(entt::entity entity) const {
			return m_Registry.any_of<T>(entity);
		}

		template<typename T, typename... Args>
		T& AddComponent(entt::entity entity, Args&&... args) {
#if ECS_PROFILE
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
#if ECS_PROFILE
			m_ComponentMemSize -= sizeof(T);
			m_ComponentCount--;
#endif
			m_Registry.remove<T>(entity);
		}

		template<typename T>
		bool TryRemoveComponent(entt::entity entity) {
			if (HasComponent<T>(entity)) {
#if ECS_PROFILE
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
#if ECS_PROFILE
			if (!HasComponent<T>(entity)) {
				m_ComponentMemSize += sizeof(T);
				m_ComponentCount++;
			}
#endif
			return m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& GetOrAddComponent(entt::entity entity, Args&&... args) {
#if ECS_PROFILE
			if (!HasComponent<T>(entity)) m_ComponentMemSize += sizeof(T);
#endif
			return m_Registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
		}
#pragma endregion

#pragma region System
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

		template<typename T>
		bool HasSystem() {
			SystemIndex systemIndex = GetSystemIndex<T>();
			for (int i = 0; i < m_Systems.size(); i++)
				if (m_Systems[i].SystemIndex == systemIndex)
					return true;
			return false;
		}

		template<typename T>
		void AddSystem() {
			m_SystemCount++;
			m_Systems.push_back({ GetSystemIndex<T>(), T::GetTopic(), T::GetExecuteOrder(), &T::Update});
			SortSystemsOnExecuteOrder();
		}

		template<typename T>
		void RemoveSystem() {
			m_SystemCount--;
			SystemIndex systemIndex = GetSystemIndex<T>();
			for (int i = 0; i < m_Systems.size(); i++)
				if (m_Systems[i].SystemIndex == systemIndex) {
					m_Systems.erase(m_Systems.begin() + i);
					break;
				}
			SortSystemsOnExecuteOrder();
		}

		void SortSystemsOnExecuteOrder() {
			std::sort(m_Systems.begin(), m_Systems.end(), [](const SystemHandle& s1, const SystemHandle& s2) {
				if (s1.Topic != s2.Topic)
					return s1.Topic > s2.Topic;
				else
					return s1.ExecuteOrder >= s2.ExecuteOrder;
			});
		}
#pragma endregion

		WorldSpecs& GetWorldSpecs() { return m_Specs; }
		void SetWorldSpecs(WorldSpecs specs);
		
		entt::registry& GetRegistry() { return m_Registry; }
		bool Paused = false;

	private:
		WorldSpecs m_Specs;
		entt::registry m_Registry;
		Array<SystemHandle> m_Systems;

#if ECS_PROFILE
		size m_ComponentMemSize = 0;
		size m_ComponentCount = 0;
		size m_SystemCount = 0;
#endif

		friend EntityHandle;
	};
}
