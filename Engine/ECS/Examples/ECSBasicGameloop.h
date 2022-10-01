#pragma once


#include "ECS/ECSGameloop.h"

namespace Pit::ECS::Example {
	class BasicGameloop : public IECSGameloop {
	public:
        virtual void Initialize(World& world) override;
        virtual void Shutdown(World& world) override;
    };
}