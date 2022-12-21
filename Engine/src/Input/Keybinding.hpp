#pragma once

#include "Core/CoreInclude.hpp"
#include "KeyCode.hpp"
#include "Serialization/KeyValueSerialization.hpp"

namespace Pit {
	struct KeyBindingEntry {
		String Name;
		KeyCode Key = KeyCode::Null;
		MouseButton MouseButton = MouseButton::Null;
	};

	class Keybindings {
	public:
		void AddBinding(const String& name, KeyCode keycode, bool setIfExist = false) {
			if (!HasBinding(name))
				m_Bindings.emplace_back(name, keycode, MouseButton::Null);
			else if (setIfExist)
				SetBinding(name, keycode);
		}

		void AddBinding(const String& name, MouseButton mouseButton, bool setIfExist = false) {
			if (!HasBinding(name))
				m_Bindings.emplace_back(name, KeyCode::Null, mouseButton);
			else if (setIfExist)
				SetBinding(name, mouseButton);
		}

		bool HasBinding(const String& name) {
			for (auto& binding : m_Bindings)
				if (binding.Name == name) return true;
			return false;
		}

		void SetBinding(const String& name, KeyCode newKeycode) {
			for (auto& binding : m_Bindings)
				if (binding.Name == name) {
					binding.Key = newKeycode;
					binding.MouseButton = MouseButton::Null;
				}
		}

		void SetBinding(const String& name, MouseButton newMouseButton) {
			for (auto& binding : m_Bindings)
				if (binding.Name == name) {
					binding.MouseButton = newMouseButton;
					binding.Key = KeyCode::Null;
				}
		}
		
		KeyCode GetBinding(const String& name) {
			for (auto& binding : m_Bindings)
				if (binding.Name == name) return binding.Key;

			return KeyCode::Null;
		}

		MouseButton GetMouseBinding(const String& name) {
			for (auto& binding : m_Bindings)
				if (binding.Name == name) return binding.MouseButton;

			return MouseButton::Null;
		}

		bool RemoveBinding(const String& name) {
			for (int i = 0; i < m_Bindings.size(); i++)
				if (m_Bindings[i].Name == name) {
					m_Bindings.erase(m_Bindings.begin() + i);
					return true;
				}
			return false;
		}

		void Serialize(const String& filepath) {
			Serialization::KeyValueSerialization out(filepath);
			for (auto& binding : m_Bindings)
				out.AddKeyValue(binding.Name, (binding.Key != KeyCode::Null ? KeyCodeToString(binding.Key) : MouseButtonToString(binding.MouseButton)));
			out.Save();
		}

		void Deserize(const String& filepath) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath);

			Array<Serialization::KeyValueDeserialization::KeyValuePair> keyValuePairs;
			Serialization::KeyValueDeserialization::Deserialize(filepath, keyValuePairs);
			for (auto& pair : keyValuePairs) {
				if (StringToKeyCode(pair.Value) != KeyCode::Null)
					AddBinding(pair.Key, StringToKeyCode(pair.Value), true);
				else
					AddBinding(pair.Key, StringToMouseButton(pair.Value), true);
			}
		}

	private:
		Array<KeyBindingEntry> m_Bindings;
	};
}