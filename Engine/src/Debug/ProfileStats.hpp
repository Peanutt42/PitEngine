#pragma once

#include "Core/Time.hpp"
#include <functional>
#include <string>

namespace Pit::Debug {
#if PIT_ENGINE_DEBUG || PIT_ENGINE_RELEASE
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
		if (std::is_same_v<T, Pit::String>) return String;
		if (std::is_same_v<T, Pit::size>) return Memory;
	}

	template<typename T>
	struct ProfileStatEntry {
		Pit::String name;
		T* pValue;
		std::function<T()> avarageFunc;
	};

	struct ProfileStatGroupEntry {
		Pit::String name;
		Array<ProfileStatEntry<float>>* pFloatStats;
		Array<ProfileStatEntry<int>>* pIntStats;
		Array<ProfileStatEntry<Pit::String>>* pStringStats;
		Array<ProfileStatEntry<size>>* pMemoryStats;
	};

	struct ProfileStatGroups {
		static Array<ProfileStatGroupEntry> s_ProfileStatGroups;
		
		template<typename T>
		static bool Register() {
			s_ProfileStatGroups.push_back(Pit::Debug::ProfileStatGroupEntry{ T::GetName(),
																			&T::s_FloatProfileStats,
																			&T::s_IntProfileStats,
																			&T::s_StringProfileStats,
																			&T::s_MemoryProfileStats });
			return true;
		}

		static Pit::String InfoToString() {
			std::stringstream result;

			result << "ProfileStatGroups:\n";
			for (auto& profileStatGroup : s_ProfileStatGroups) {
				result << "\t" << profileStatGroup.name << ":\n";
				for (auto& floatStat : *profileStatGroup.pFloatStats)
					result << "\t\t" << floatStat.name << ": " << floatStat.avarageFunc() << '\n';

				for (auto& intStat : *profileStatGroup.pIntStats)
					result << "\t\t" << intStat.name << ": " << intStat.avarageFunc() << '\n';

				for (auto& stringStat : *profileStatGroup.pStringStats)
					result << "\t\t" << stringStat.name << ": " << stringStat.avarageFunc() << '\n';

				for (auto& memoryStat : *profileStatGroup.pMemoryStats)
					result << "\t\t" << memoryStat.name << ": " << memoryStat.avarageFunc() << '\n';
			}

			return result.str();
		}
	};

	// usage: DECLARE_PROFILE_STAT_GROUP(Example, "Example");
#define DECLARE_PROFILE_STAT_GROUP(name, statName)																		\
	struct STAT_GROUP_##name {																							\
		static Pit::String GetName() {																					\
			return #statName;																							\
		}																												\
		static bool Registered;																							\
		static Pit::Array<Pit::Debug::ProfileStatEntry<float>> s_FloatProfileStats;									\
		static Pit::Array<Pit::Debug::ProfileStatEntry<int>> s_IntProfileStats;										\
		static Pit::Array<Pit::Debug::ProfileStatEntry<Pit::String>> s_StringProfileStats;								\
		static Pit::Array<Pit::Debug::ProfileStatEntry<Pit::size>> s_MemoryProfileStats;									\
		template<typename T>																							\
		static bool Register() {																						\
			if constexpr (T::Type == Pit::Debug::Float) {																\
				s_FloatProfileStats.emplace_back(T::GetName(),															\
												 &T::GetValue(),														\
												 [](){return T::GetAvarageValue();});							\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::Int) {																	\
				s_IntProfileStats.emplace_back(T::GetName(),															\
											   &T::GetValue(),															\
											   [](){return T::GetAvarageValue();});								\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::String) {																\
				s_StringProfileStats.emplace_back(T::GetName(),															\
												  &T::GetValue(),														\
												  [](){return T::GetAvarageValue();});						\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::Memory) {																\
				s_MemoryProfileStats.emplace_back(T::GetName(),															\
												  &T::GetValue(),														\
												  [](){return T::GetAvarageValue();});							\
				return true;																							\
			}																											\
		}																												\
	};																													\
	bool STAT_GROUP_##name::Registered = Pit::Debug::ProfileStatGroups::Register<STAT_GROUP_##name>();					\
	Pit::Array<Pit::Debug::ProfileStatEntry<float>> STAT_GROUP_##name::s_FloatProfileStats;							\
	Pit::Array<Pit::Debug::ProfileStatEntry<int>> STAT_GROUP_##name::s_IntProfileStats;								\
	Pit::Array<Pit::Debug::ProfileStatEntry<Pit::String>> STAT_GROUP_##name::s_StringProfileStats;						\
	Pit::Array<Pit::Debug::ProfileStatEntry<Pit::size>> STAT_GROUP_##name::s_MemoryProfileStats


#define DECLARE_EXTERN_PROFILE_STAT_GROUP(name, statName)																\
	struct STAT_GROUP_##name {																							\
		static Pit::String GetName() {																					\
			return #statName;																							\
		}																												\
		static bool Registered;																							\
		static Pit::Array<Pit::Debug::ProfileStatEntry<float>> s_FloatProfileStats;									\
		static Pit::Array<Pit::Debug::ProfileStatEntry<int>> s_IntProfileStats;										\
		static Pit::Array<Pit::Debug::ProfileStatEntry<Pit::String>> s_StringProfileStats;								\
		static Pit::Array<Pit::Debug::ProfileStatEntry<Pit::size>> s_MemoryProfileStats;									\
		template<typename T>																							\
		static bool Register() {																						\
			if constexpr (T::Type == Pit::Debug::Float) {																\
				s_FloatProfileStats.emplace_back(T::GetName(),															\
												 &T::GetValue(),														\
												 [](){return T::GetAvarageValue();});							\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::Int) {																	\
				s_IntProfileStats.emplace_back(T::GetName(),															\
											   &T::GetValue(),															\
											   [](){return T::GetAvarageValue();});								\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::String) {																\
				s_StringProfileStats.emplace_back(T::GetName(),															\
												  &T::GetValue(),														\
												  [](){return T::GetAvarageValue();});						\
				return true;																							\
			}																											\
			if constexpr (T::Type == Pit::Debug::Memory) {																\
				s_MemoryProfileStats.emplace_back(T::GetName(),															\
												  &T::GetValue(),														\
												  [](){return T::GetAvarageValue();});							\
				return true;																							\
			}																											\
		}																												\
	}	

#define DEFINE_EXTERN_PROFILE_STAT_GROUP(name)																			\
	bool STAT_GROUP_##name::Registered = Pit::Debug::ProfileStatGroups::Register<STAT_GROUP_##name>();					\
	Pit::Array<Pit::Debug::ProfileStatEntry<float>> STAT_GROUP_##name::s_FloatProfileStats;							\
	Pit::Array<Pit::Debug::ProfileStatEntry<int>> STAT_GROUP_##name::s_IntProfileStats;								\
	Pit::Array<Pit::Debug::ProfileStatEntry<Pit::String>> STAT_GROUP_##name::s_StringProfileStats;						\
	Pit::Array<Pit::Debug::ProfileStatEntry<Pit::size>> STAT_GROUP_##name::s_MemoryProfileStats


	// usage: DECLARE_PROFILE_STAT(ExampleGroup, "Example");
#define DECLARE_PROFILE_STAT(type, name, statGroup, statName, default_val)												\
	struct STAT_##name {																								\
		static Pit::String GetName() {																					\
			return #statName;																							\
		}																												\
		template<typename T>																							\
		static constexpr bool IsInGroup() {																				\
			return std::is_same_v<T, STAT_GROUP_##statGroup>;															\
		}																												\
		static constexpr Pit::Debug::ProfileStatType Type = Pit::Debug::GetProfileType<type>();							\
		static void SetValue(type value) {																				\
			Value = value;																								\
			static Pit::size ValueRecordIndex = 0, shouldRecordTime = 0;													\
			if (shouldRecordTime < 150) {																				\
				shouldRecordTime++;																						\
				return;																									\
			}																											\
			shouldRecordTime = 0;																						\
			ValueRecord[ValueRecordIndex % 10] = value;																	\
			ValueRecordIndex++;																							\
		}																												\
		static type& GetValue() {																						\
			return Value;																								\
		}																												\
		static type GetAvarageValue() {																				\
			if constexpr(std::is_same_v<type, Pit::String>) return Value;												\
			type sum = ValueRecord[0] + ValueRecord[1] + ValueRecord[2] + ValueRecord[3] + ValueRecord[4] +				\
				       ValueRecord[5] + ValueRecord[6] + ValueRecord[7] + ValueRecord[8] + ValueRecord[9];				\
			return sum / 10;																							\
		}																												\
	private:																											\
		static bool Registered;																							\
		static type Value;																								\
		static type ValueRecord[10];																					\
	};																													\
	type STAT_##name::Value = default_val;																				\
	bool Registered = STAT_GROUP_##statGroup::Register<STAT_##name>();													\
	type STAT_##name::ValueRecord[10]

#define DECLARE_EXTERN_PROFILE_STAT(type, name, statGroup, statName)													\
	struct STAT_##name {																								\
		static Pit::String GetName() {																					\
			return #statName;																							\
		}																												\
		template<typename T>																							\
		static constexpr bool IsInGroup() {																				\
			return std::is_same_v<T, STAT_GROUP_##statGroup>;															\
		}																												\
		static void SetValue(type value) {																				\
			Value = value;																								\
			static Pit::size ValueRecordIndex = 0, shouldRecordTime = 0;													\
			if (shouldRecordTime < 150) {																				\
				shouldRecordTime++;																						\
				return;																									\
			}																											\
			shouldRecordTime = 0;																						\
			ValueRecord[ValueRecordIndex % 10] = value;																	\
			ValueRecordIndex++;																							\
		}																												\
		static type& GetValue() {																						\
			return Value;																								\
		}																												\
		static type GetAvarageValue() {																					\
			if constexpr(std::is_same_v<type, Pit::String>) return Value;												\
			type sum = ValueRecord[0] + ValueRecord[1] + ValueRecord[2] + ValueRecord[3] + ValueRecord[4] +				\
				       ValueRecord[5] + ValueRecord[6] + ValueRecord[7] + ValueRecord[8] + ValueRecord[9];				\
			return sum / 10;																							\
		}																												\
		static constexpr Pit::Debug::ProfileStatType Type = Pit::Debug::GetProfileType<type>();							\
	private:																											\
		static type Value;																								\
		static bool Registered;																							\
		static type ValueRecord[10];																					\
	}

#define DEFINE_EXTERN_PROFILE_STAT(type, name, statGroup, default_val)													\
	bool STAT_##name::Registered = STAT_GROUP_##statGroup::Register<STAT_##name>();										\
	type STAT_##name::Value = default_val;																				\
	type STAT_##name::ValueRecord[10]



	// Default ProfileStat types
#define DECLARE_PROFILE_STAT_FUNC(name, statGroup, statName)															\
	DECLARE_PROFILE_STAT_FLOAT(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_FUNC(name, statGroup, statName)														\
	DECLARE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup, statName)															
#define DEFINE_EXTERN_PROFILE_STAT_FUNC(name, statGroup)																\
	DEFINE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup)

#define DECLARE_PROFILE_STAT_FLOAT(name, statGroup, statName)															\
	DECLARE_PROFILE_STAT(float, name, statGroup, statName, 0.f)
#define DECLARE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup, statName)													\
	DECLARE_EXTERN_PROFILE_STAT(float, name, statGroup, statName)															
#define DEFINE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup)																\
	DEFINE_EXTERN_PROFILE_STAT(float, name, statGroup, 0.f)


#define DECLARE_PROFILE_STAT_INT(name, statGroup, statName)																\
	DECLARE_PROFILE_STAT(int, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_INT(name, statGroup, statName)														\
	DECLARE_EXTERN_PROFILE_STAT(int, name, statGroup, statName)															
#define DEFINE_EXTERN_PROFILE_STAT_INT(name, statGroup)																	\
	DEFINE_EXTERN_PROFILE_STAT(int, name, statGroup, 0)


#define DECLARE_PROFILE_STAT_STRING(name, statGroup, statName)															\
	DECLARE_PROFILE_STAT(Pit::String, name, statGroup, statName, "")
#define DECLARE_EXTERN_PROFILE_STAT_STRING(name, statGroup, statName)													\
	DECLARE_EXTERN_PROFILE_STAT(Pit::String, name, statGroup, statName)															
#define DEFINE_EXTERN_PROFILE_STAT_STRING(name, statGroup)																\
	DEFINE_EXTERN_PROFILE_STAT(Pit::String, name, statGroup, "")


#define DECLARE_PROFILE_STAT_MEMORY(name, statGroup, statName)															\
	DECLARE_PROFILE_STAT(size, name, statGroup, statName, 0)
#define DECLARE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup, statName)													\
	DECLARE_EXTERN_PROFILE_STAT(size, name, statGroup, statName)															
#define DEFINE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup)																\
	DEFINE_EXTERN_PROFILE_STAT(size, name, statGroup, 0)




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

#define SCOPE_STAT(name) \
	Debug::ProfileStatTimer __profileScopeStatTime([&](float millis){ STAT_##name::SetValue(millis); });
#define SCOPE_STAT_ADD(name) \
	Debug::ProfileStatTimer __profileScopeStatTime([&](float millis){ STAT_##name::SetValue(STAT_##name::GetValue() + millis); });

#define STAT_RESET(name) \
	STAT_##name::SetValue(NULL)


#define GET_STAT_VALUE(name) \
	STAT_##name::Value

#define SET_STAT_VALUE(name, value) \
	STAT_##name::Value = value

#else
#define DECLARE_PROFILE_STAT_GROUP(name, statName)
#define DECLARE_EXTERN_PROFILE_STAT_GROUP(name, statName)
#define DEFINE_EXTERN_PROFILE_STAT_GROUP(name)
#define DECLARE_PROFILE_STAT(type, name, statGroup, statName, default_val)
#define DECLARE_EXTERN_PROFILE_STAT(type, name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT(type, name, statGroup, default_val)
#define DECLARE_PROFILE_STAT_FUNC(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_FUNC(name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT_FUNC(name, statGroup)
#define DECLARE_PROFILE_STAT_FLOAT(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT_FLOAT(name, statGroup)
#define DECLARE_PROFILE_STAT_INT(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_INT(name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT_INT(name, statGroup)
#define DECLARE_PROFILE_STAT_STRING(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_STRING(name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT_STRING(name, statGroup)
#define DECLARE_PROFILE_STAT_MEMORY(name, statGroup, statName)
#define DECLARE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup, statName)
#define DEFINE_EXTERN_PROFILE_STAT_MEMORY(name, statGroup)

#define SCOPE_STAT(name)	{}
#define SCOPE_STAT_ADD(name)	{}
#define STAT_RESET(name)	{}
#define GET_STAT_VALUE(name)	{}
#define SET_STAT_VALUE(name, value)	{}
#endif
}



DECLARE_EXTERN_PROFILE_STAT_GROUP(General, "General");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Game, "Game");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Editor, "Editor");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Rendering, "Rendering");
DECLARE_EXTERN_PROFILE_STAT_GROUP(UIRendering, "UIRendering");
DECLARE_EXTERN_PROFILE_STAT_GROUP(ECS, "ECS");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Audio, "Audio");
DECLARE_EXTERN_PROFILE_STAT_GROUP(Networking, "Networking");