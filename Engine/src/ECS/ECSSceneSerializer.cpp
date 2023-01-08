#include "pch.hpp"
#include "Core/CoreInclude.hpp"
#include "Core/Engine.hpp"
#include "ECSSceneSerializer.hpp"
#include "Rendering/Camera.hpp"
#include "ECSEntityHandle.hpp"
#include "ECSComponents.hpp"
#include "Serialization/YAMLSerializer.hpp"

namespace YAML {
	#pragma region YAML_conversions
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Pit::UUID> {
		static Node encode(const Pit::UUID& uuid) {
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Pit::UUID& uuid) {
			uuid = node.as<uint64_t>();
			return true;
		}
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	#pragma endregion
	}

namespace Pit::ECS {
	template<typename T>
	static void SerializeComponent(YAML::Emitter& out, EntityHandle entity, const String& componentName, std::function<void(T&)> uifunc) {
		if (entity.HasComponent<T>()) {
			out << YAML::Key << componentName;
			out << YAML::BeginMap;

			T& component = entity.GetComponent<T>();
			uifunc(component);

			out << YAML::EndMap;
		}
	}

	static void SerializeEntity(YAML::Emitter& out, EntityHandle entity) {
		PIT_ENGINE_ASSERT(ECS, entity.HasComponent<UUIDComponent>(), "");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetComponent<UUIDComponent>().Id;

		if (entity.HasComponent<NameComponent>()) {
			out << YAML::Key << "Name" << YAML::Value << entity.GetComponent<NameComponent>().Name;
		}
		SerializeComponent<TransformComponent>(out, entity, "Transform", [&](TransformComponent& transformComponent) {
			out << YAML::Key << "Position" << YAML::Value << transformComponent.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;
		});

		out << YAML::EndMap;
	}

	const bool SceneSerializer::Serialize(const std::filesystem::path& filepath, Scene& scene) {
		PIT_PROFILE_FUNCTION();

		String filepathStr = filepath.string();
		String sceneName = scene.GetName();
		PIT_ENGINE_INFO(ECS, "Saving ECS scene '{}' to '{}'", sceneName, filepathStr);

		ScopedTimer t("Saving ECS scene " + sceneName + " to " + filepathStr);

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Scene" << YAML::Value << sceneName;
		out << YAML::Key << "Camera";
		const Rendering::Camera& cam = scene.GetCamera();
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << cam.Position;
		out << YAML::Key << "Rotation" << YAML::Value << cam.Rotation;
		out << YAML::Key << "Fov" << YAML::Value << cam.Fov;
		out << YAML::Key << "NearPlane" << YAML::Value << cam.NearPlane;
		out << YAML::Key << "FarPlane" << YAML::Value << cam.FarPlane;
		out << YAML::EndMap;

		out << YAML::Key << "Entities" << YAML::Value;
		out << YAML::BeginSeq;

		scene.Each([&](entt::entity entityId) {
			EntityHandle entity(&scene, entityId);
			if (entity) {
				SerializeEntity(out, entity);
			}
		});

		out << YAML::EndSeq;

		out << YAML::EndMap;

		Serialization::SaveYamlToFile(filepath, out);

		return true;
	}

	template<typename T>
	static void DeserializeComponent(YAML::iterator::value_type& entityNode, const String& name, std::function<void(YAML::Node&)> uifunc) {
		auto component = entityNode[name];
		if (component) {
			uifunc(component);
		}
	}

	const bool SceneSerializer::Deserialize(const std::filesystem::path& filepath, Scene& scene) {
		PIT_PROFILE_FUNCTION();

		String filepathStr = filepath.string();

		YAML::Node data;
		try {
			data = YAML::LoadFile(filepathStr);
		}
		catch (YAML::ParserException e) {
			PIT_ENGINE_ERR(ECS, "Failed to load .pitscene file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		PIT_ENGINE_INFO(ECS, "Loading ECS scene '{}' from '{}'", sceneName, filepathStr);

		ScopedTimer t("Loading ECS scene " + sceneName + " from " + filepathStr);


		auto entities = data["Entities"];
		if (entities) {
			for (auto entityNode : entities) {
				EntityHandle entity = scene.CreateEntity();
				entity.GetComponent<UUIDComponent>().Id = (UUID)entityNode["Entity"].as<uint64_t>();

				entity.GetComponent<NameComponent>().Name = entityNode["Name"].as<String>();

				DeserializeComponent<TransformComponent>(entityNode, "Transform", [&](YAML::Node& transformNode) {
					TransformComponent& transform = entity.GetComponent<TransformComponent>();
					transform.Position = transformNode["Position"].as<glm::vec3>();
					transform.Rotation = transformNode["Rotation"].as<glm::vec3>();
					transform.Scale = transformNode["Scale"].as<glm::vec3>();
				});
		
			}
		}

		return true;
	}


}