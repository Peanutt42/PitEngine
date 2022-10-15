#pragma once

#include "Main/Time.hpp"
#include <functional>
#include <string>

namespace Pit::Debug {
	// usage: DECLARE_PROFILE_STAT_GROUP(Example, "Example");
#define DECLARE_PROFILE_STAT_GROUP(name, statName)				\
	struct STAT_GROUP_##name {										\
		static std::string GetName() {							\
			return #statName;									\
		}														\
	}															

#define DECLARE_EXTERN_PROFILE_STAT_GROUP(name, statName)				\
	struct STAT_GROUP_##name {										\
		static std::string GetName() {							\
			return #statName;									\
		}														\
	}															


	enum ProfileStatType {
		Float,
		Int,
		String,
		Memory
	};
	template<typename T>
	ProfileStatType GetProfileType() {
		if (std::is_same_v<T, float>) return Float;
		if (std::is_same_v<T, int>) return Int;
		if (std::is_same_v<T, std::string>) return String;
		if (std::is_same_v<T, size_t>) return Memory;
	}

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
		static Debug::ProfileStatType GetProfileType() {									\
			return Debug::GetProfileType<type>();											\
		}																			\
		static type Value;															\
	};																				\
	type STAT_##name::Value = default_val

#define DECLARE_EXTERN_PROFILE_STAT(type, name, statGroup, statName)				\
	struct STAT_##name {															\
		static std::string GetName() {												\
			return #statName;														\
		}																			\
		template<typename T>														\
		static constexpr bool IsInGroup() {											\
			return std::is_same_v<T, STAT_GROUP_##statGroup>;						\
		}																			\
		static Debug::ProfileStatType GetProfileType() {									\
			return Debug::GetProfileType<type>();											\
		}																			\
		static type Value;															\
	}

#define DEFINE_EXTERN_PROFILE_STAT(type, name, default_val)							\
	type STAT_##name::Value = default_val



	// Default ProfileStat types
#define DECLARE_PROFILE_STAT_FLOAT(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(float, name, statGroup, statName, 0.f)
#define DECLARE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(float, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_FLOAT(name)										\
	DEFINE_EXTERN_PROFILE_STAT(float, name, 0.f)


#define DECLARE_PROFILE_STAT_INT(name, statGroup, statName)							\
	DECLARE_PROFILE_STAT(int, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_INT(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(int, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_INT(name)										\
	DEFINE_EXTERN_PROFILE_STAT(int, name, 0)


#define DECLARE_PROFILE_STAT_STRING(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(std::string, name, statGroup, statName, "")
#define DECLARE_EXTERN_PROFILE_STAT_STRING(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(std::string, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_STRING(name)										\
	DEFINE_EXTERN_PROFILE_STAT(std::string, name, "")


#define DECLARE_PROFILE_STAT_MEMORY(name, statGroup, statName)						\
	DECLARE_PROFILE_STAT(size_t, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup, statName)				\
	DECLARE_EXTERN_PROFILE_STAT(size_t, name, statGroup, statName)						
#define DEFINE_EXTERN_PROFILE_STAT_MEMORY(name)										\
	DEFINE_EXTERN_PROFILE_STAT(size_t, name, 0)




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