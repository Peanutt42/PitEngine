#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "RenderingSystem.hpp"
#include "RenderingSubmodule.hpp"

using namespace Pit;

void RenderingSubmodule::Init() {
	Renderer = new Rendering::Renderer();
	RenderingSystem = new Rendering::RenderingSystem();
	CurrentCamera = new Rendering::Camera();
	CurrentCamera->ProjectionMode = Rendering::Camera::Projection::Perspective;
	CurrentCamera->FOV = 50;
	CurrentCamera->AspectRatio = Engine::Rendering()->GetRenderer()->SwapChain->extentAspectRatio();
	CurrentCamera->NearClip = 0.1f;
	CurrentCamera->FarClip = 500;
	CurrentCamera->SetProjection();
	CurrentCamera->SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
	Rendering::RenderEntitiesSystem::CameraToUse = CurrentCamera;
	UIRenderer = new UI::Renderer(Renderer);
	Engine::ECS()->GetEcsWorld().AddSystem<Rendering::RenderEntitiesSystem>();
}

void RenderingSubmodule::Shutdown() {
	delete UIRenderer;
	delete RenderingSystem;
	delete Renderer;
}

void RenderingSubmodule::Update() {
	float aspect = Engine::Rendering()->GetRenderer()->SwapChain->extentAspectRatio();
	CurrentCamera->ProjectionMode = Rendering::Camera::Projection::Perspective;
	CurrentCamera->FOV = 50;
	CurrentCamera->AspectRatio = aspect;
	CurrentCamera->NearClip = 0.1f;
	CurrentCamera->FarClip = 500;
	CurrentCamera->SetProjection();
	Renderer->Update();
}