#pragma once

#include "ECSWorld.h"

namespace Pit::ECS {
    class IECSGameloop {
    public:
        virtual void Initialize(World& world) = 0;
        virtual void Shutdown(World& world) = 0;
    };
}