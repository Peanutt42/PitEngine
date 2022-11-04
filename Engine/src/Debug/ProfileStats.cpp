#include "pch.hpp"
#include "ProfileStats.hpp"


#if DEBUG || RELEASE
Pit::Array<Pit::Debug::ProfileStatGroupEntry> Pit::Debug::ProfileStatGroups::s_ProfileStatGroups;
#endif

DEFINE_EXTERN_PROFILE_STAT_GROUP(General);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Game);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Editor);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Rendering);
DEFINE_EXTERN_PROFILE_STAT_GROUP(UIRendering);
DEFINE_EXTERN_PROFILE_STAT_GROUP(ECS);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Audio);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Networking);