#pragma once

#include "Core/CoreInclude.hpp"
#include "KeyCode.hpp"
#include <yaml-cpp/yaml.h>

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
			std::ofstream fout;
			fout.open(filepath);
			for (auto& binding : m_Bindings)
				fout << binding.Name << ": " << (binding.Key != KeyCode::Null ? KeyCodeToString(binding.Key) : MouseButtonToString(binding.MouseButton)) << '\n';

			fout.close();
		}

		void Deserize(const String& filepath) {
			if (!std::filesystem::exists(filepath)) Serialize(filepath);

			std::ifstream fin;
			fin.open(filepath);
			std::string line;
			while (getline(fin, line)) {
				// example: _THIS_IS_A_TEST_BINDING: Space
				std::string key, value;
				bool keyFinished = false;
				for (auto c : line) {
					if (c == 0) break;
					if (c == ':')
						keyFinished = true;
					else if (keyFinished && c != ' ')
						value += c;
					else if (c != ' ')
						key += c;
				}
				if (StringToKeyCode(value) != KeyCode::Null)
					AddBinding(key, StringToKeyCode(value), true);
				else
					AddBinding(key, StringToMouseButton(value), true);
			}
			fin.close();
		}

	private:
		Array<KeyBindingEntry> m_Bindings;
	};
}