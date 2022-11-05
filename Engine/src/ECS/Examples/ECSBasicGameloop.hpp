#pragma once


#include "ECS/ECSGameloop.hpp"

namespace Pit::ECS::Example {
	class BasicGameloop : public IECSGameloop {
	public:
        virtual void Initialize(World& world) override;
        virtual void Shutdown(World& world) override;
        virtual void Update(World& world) override;
    };
}