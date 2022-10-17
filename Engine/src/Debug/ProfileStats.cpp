#include "Main/CoreInclude.hpp"
#include "ProfileStats.hpp"

std::vector<Pit::Debug::ProfileStatGroupEntry> Pit::Debug::ProfileStatGroups::s_ProfileStatGroups;

DEFINE_EXTERN_PROFILE_STAT_GROUP(General);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Game);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Editor);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Rendering);
DEFINE_EXTERN_PROFILE_STAT_GROUP(ECS);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Audio);
DEFINE_EXTERN_PROFILE_STAT_GROUP(Networking);