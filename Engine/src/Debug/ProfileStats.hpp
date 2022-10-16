#pragma once

#include "Main/Time.hpp"
#include <functional>
#include <string>

namespace Pit::Debug {

	enum ProfileStatType {
		Float,
		Int,
		String,
		Memory
	};
	template<typename T>
	static constexpr const ProfileStatType GetProfileType() {
		if (std::is_same_v<T, float>) return Float;
		if (std::is_same_v<T, int>) return Int;
		if (std::is_same_v<T, std::string>) return String;
		if (std::is_same_v<T, size_t>) return Memory;
	}

	template<typename T>
	struct ProfileStatEntry {
		std::string name;
		T* pValue;
	};


	// usage: DECLARE_PROFILE_STAT_GROUP(Example, "Example");
#define DECLARE_PROFILE_STAT_GROUP(name, statName)									\
	struct STAT_GROUP_##name {														\
		static std::string GetName() {												\
			return #statName;														\
		}																			\
		static std::vector<Pit::Debug::ProfileStatEntry<float>> s_FloatProfileStats;			\
		static std::vector<Pit::Debug::ProfileStatEntry<int>> s_IntProfileStats;				\
		static std::vector<Pit::Debug::ProfileStatEntry<std::string>> s_StringProfileStats;		\
		static std::vector<Pit::Debug::ProfileStatEntry<size_t>> s_MemoryProfileStats;			\
		template<typename T>														\
		static bool Register() {													\
			if constexpr (T::Type == Pit::Debug::Float) {							\
				s_FloatProfileStats.push_back(Pit::Debug::ProfileStatEntry<float>{T::GetName(), &T::Value});			\
				return true;														\
			}																		\
			if constexpr (T::Type == Pit::Debug::Int) {								\
				s_IntProfileStats.push_back(Pit::Debug::ProfileStatEntry<int>{T::GetName(), &T::Value});				\
				return true;														\
			}																		\
			if constexpr (T::Type == Pit::Debug::String) {							\
				s_StringProfileStats.push_back(Pit::Debug::ProfileStatEntry<std::string>{T::GetName(), &T::Value});			\
				return true;														\
			}																		\
			if constexpr (T::Type == Pit::Debug::Memory) {							\
				s_MemoryProfileStats.push_back(Pit::Debug::ProfileStatEntry<size_t>{T::GetName(), &T::Value});			\
				return true;														\
			}																		\
		}																			\
	std::vector<Pit::Debug::ProfileStatEntry<float>> STAT_GROUP_##name::s_FloatProfileStats;	\
	std::vector<Pit::Debug::ProfileStatEntry<int>> STAT_GROUP_##name::s_IntProfileStats;		\
	std::vector<Pit::Debug::ProfileStatEntry<std::string>> STAT_GROUP_##name::s_StringProfileStats;	\
	std::vector<Pit::Debug::ProfileStatEntry<size_t>> STAT_GROUP_##name::s_MemoryProfileStats


#define DECLARE_EXTERN_PROFILE_STAT_GROUP(name, statName)							\
	struct STAT_GROUP_##name {														\
		static std::string GetName() {												\
			return #statName;														\
		}																			\
		static std::vector<Pit::Debug::ProfileStatEntry<float>> s_FloatProfileStats;			\
		static std::vector<Pit::Debug::ProfileStatEntry<int>> s_IntProfileStats;				\
		static std::vector<Pit::Debug::ProfileStatEntry<std::string>> s_StringProfileStats;		\
		static std::vector<Pit::Debug::ProfileStatEntry<size_t>> s_MemoryProfileStats;			\
		template<typename T>														\
		static bool Register() {													\
			if constexpr (T::Type == Pit::Debug::Float) {						\
				s_FloatProfileStats.push_back(Pit::Debug::ProfileStatEntry<float>{T::GetName(), &T::Value});			\
				return true;												\
			}																		\
			if constexpr (T::Type == Pit::Debug::Int) {							\
				s_IntProfileStats.push_back(Pit::Debug::ProfileStatEntry<int>{T::GetName(), &T::Value});			\
				return true;												\
			}																		\
			if constexpr (T::Type == Pit::Debug::String) {							\
				s_StringProfileStats.push_back(Pit::Debug::ProfileStatEntry<std::string>{T::GetName(), &T::Value});			\
				return true;													\
			}																		\
			if constexpr (T::Type == Pit::Debug::Memory) {						\
				s_MemoryProfileStats.push_back(Pit::Debug::ProfileStatEntry<size_t>{T::GetName(), &T::Value});			\
				return true;												\
			}																		\
		}																			\
	}	

#define DEFINE_EXTERN_PROFILE_STAT_GROUP(name)										\
	std::vector<Pit::Debug::ProfileStatEntry<float>> STAT_GROUP_##name::s_FloatProfileStats;	\
	std::vector<Pit::Debug::ProfileStatEntry<int>> STAT_GROUP_##name::s_IntProfileStats;		\
	std::vector<Pit::Debug::ProfileStatEntry<std::string>> STAT_GROUP_##name::s_StringProfileStats;	\
	std::vector<Pit::Debug::ProfileStatEntry<size_t>> STAT_GROUP_##name::s_MemoryProfileStats


	// usage: DECLARE_PROFILE_STAT(ExampleGroup, "Example");
#define DECLARE_PROFILE_STAT(type, name, statGroup, statName, default_val)			\
	struct STAT_##name {															\
		static std::string GetName() {												\
			return #statName;														\
		}																			\
		template<typename T>														\
		static constexpr bool IsInGroup() {											\
			return std::is_same_v<T, STAT_GROUP_##statGroup>;						\
		}																			\
		static constexpr Pit::Debug::ProfileStatType Type = Pit::Debug::GetProfileType<type>();									\
		static type Value;															\
		static bool Registered;														\
	};																				\
	type STAT_##name::Value = default_val;											\
	bool Registered = STAT_GROUP_##statGroup::Register<STAT_##name>()

#define DECLARE_EXTERN_PROFILE_STAT(type, name, statGroup, statName)				\
	struct STAT_##name {															\
		static std::string GetName() {												\
			return #statName;														\
		}																			\
		template<typename T>														\
		static constexpr bool IsInGroup() {											\
			return std::is_same_v<T, STAT_GROUP_##statGroup>;						\
		}																			\
		static constexpr Pit::Debug::ProfileStatType Type = Pit::Debug::GetProfileType<type>();									\
		static type Value;															\
		static bool Registered;														\
	}

#define DEFINE_EXTERN_PROFILE_STAT(type, name, statGroup, default_val)				\
	type STAT_##name::Value = default_val;											\
	bool STAT_##name::Registered = STAT_GROUP_##statGroup::Register<STAT_##name>();	\



	// Default ProfileStat types
#define DECLARE_PROFILE_STAT_FLOAT(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(float, name, statGroup, statName, 0.f)
#define DECLARE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(float, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup)							\
	DEFINE_EXTERN_PROFILE_STAT(float, name, statGroup, 0.f)


#define DECLARE_PROFILE_STAT_INT(name, statGroup, statName)							\
	DECLARE_PROFILE_STAT(int, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_INT(name, statGroup, statName)					\
	DECLARE_EXTERN_PROFILE_STAT(int, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_INT(name, statGroup)								\
	DEFINE_EXTERN_PROFILE_STAT(int, name, statGroup, 0)


#define DECLARE_PROFILE_STAT_STRING(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(std::string, name, statGroup, statName, "")
#define DECLARE_EXTERN_PROFILE_STAT_STRING(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(std::string, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_STRING(name, statGroup)							\
	DEFINE_EXTERN_PROFILE_STAT(std::string, name, statGroup, "")


#define DECLARE_PROFILE_STAT_MEMORY(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(size_t, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(size_t, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup)							\
	DEFINE_EXTERN_PROFILE_STAT(size_t, name, statGroup, 0)




	class ProfileStatTimer : public Timer {
	public:
		ProfileStatTimer(std::function<void(float)> finishCallback)
			: m_FinishedCallback(finishCallback) {


		}

		~ProfileStatTimer() {
			m_FinishedCallback(m_Timer.ElapsedMillis());
		}

	private:
		Timer m_Timer;
		std::function<void(float)> m_FinishedCallback;
	};

#define SCOPE_STAT(name)	\
	Debug::ProfileStatTimer __profileScopeStatTime([&](float millis){ STAT_##name::Value = millis; });


#define GET_STAT_VALUE(name) \
	STAT_##name::Value

#define SET_STAT_VALUE(name, value) \
	STAT_##name::Value = value

}





DECLARE_EXTERN_PROFILE_STAT_GROUP(General, "General");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Game, "Game");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Editor, "Editor");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Rendering, "Rendering");
DECLARE_EXTERN_PROFILE_STAT_GROUP(ECS, "ECS");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Audio, "Audio");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Networking, "Networking");