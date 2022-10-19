#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "RenderingSystem.hpp"
#include "RenderingSubmodule.hpp"

using namespace Pit;

void RenderingSubmodule::Init() {
	Renderer = new Rendering::Renderer();
	RenderingSystem = new Rendering::RenderingSystem();
	CurrentCamera = new Rendering::Camera();
	CurrentCamera->ProjectionMode = Rendering::Camera::Projection::Orthographic;
	CurrentCamera->Left = -1;
	CurrentCamera->Right = 1;
	CurrentCamera->Top = -1;
	CurrentCamera->Bottom = 1;
	CurrentCamera->NearClip = -1;
	CurrentCamera->FarClip = 1;
	CurrentCamera->SetProjection();
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
	CurrentCamera->FOV = 70;
	CurrentCamera->AspectRatio = aspect;
	CurrentCamera->NearClip = 0.1f;
	CurrentCamera->FarClip = 10;
	CurrentCamera->SetProjection();
	Renderer->Update();
}