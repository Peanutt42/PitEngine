#include "pch.hpp"
#include "Core/Engine.hpp"
#include "RenderingSystem.hpp"
#include "RenderingSubmodule.hpp"
#include "C:\dev\cpp\PitEngine\Editor\src\EditorCameraController.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/ECSEntityHandle.hpp"

using namespace Pit;

static ECS::EntityHandle CameraEntity = {nullptr, entt::null};

void RenderingSubmodule::Init() {
	Renderer = new Rendering::Renderer();
	RenderingSystem = new Rendering::RenderingSystem();
	CameraEntity = Engine::ECS()->GetEcsWorld().CreateEntity();
	CurrentCamera = new Rendering::Camera();
	CurrentCamera->ProjectionMode = Rendering::Camera::Projection::Perspective;
	CurrentCamera->FOV = 70;
	CurrentCamera->AspectRatio = Engine::Rendering()->Renderer->SwapChain->extentAspectRatio();
	CurrentCamera->NearClip = 0.01f;
	CurrentCamera->FarClip = 5000;
	CurrentCamera->SetProjection();
	CurrentCamera->SetViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));
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
	float aspect = Engine::Rendering()->Renderer->SwapChain->extentAspectRatio();
	CurrentCamera->ProjectionMode = Rendering::Camera::Projection::Perspective;
	CurrentCamera->FOV = 70;
	CurrentCamera->AspectRatio = aspect;
	CurrentCamera->NearClip = 0.01f;
	CurrentCamera->FarClip = 5000;
	auto& camEntityTransform = CameraEntity.GetComponent<ECS::TransformComponent>();
	Editor::EditorCameraController::MoveInPlaneXZ(camEntityTransform.position, camEntityTransform.rotation);
	CurrentCamera->SetViewYXZ(camEntityTransform.position, camEntityTransform.rotation);
	CurrentCamera->SetProjection();
	Renderer->Update();
}