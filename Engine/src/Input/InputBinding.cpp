#include "pch.hpp"
#include "InputBinding.hpp"
#include "Serialization/YAMLSerializer.hpp"

namespace Pit {
	void InputBinding::AddBinding(const String& name, KeyCode keycode, bool setIfExist) {
		if (!HasBinding(name))
			m_Bindings.emplace_back(name, keycode, MouseButton::None);
		else if (setIfExist)
			SetBinding(name, keycode);
	}
	
	void InputBinding::AddBinding(const String& name, MouseButton mouseButton, bool setIfExist) {
		if (!HasBinding(name))
			m_Bindings.emplace_back(name, KeyCode::None, mouseButton);
		else if (setIfExist)
			SetBinding(name, mouseButton);
	}

	void InputBinding::AddBinding(const String& name, ControllerButton controllerButton, bool setIfExist) {
		if (!HasBinding(name))
			m_Bindings.emplace_back(name, KeyCode::None, MouseButton::None, controllerButton);
		else if (setIfExist)
			SetBinding(name, controllerButton);
	}

	void InputBinding::AddAxisBinding(const String& name, const KeyAxis& keyAxis, bool setIfExists) {
		if (!HasAxisBinding(name))
			m_AxisBindings.emplace_back(name, keyAxis, ControllerAxis::None, false);
		else if (setIfExists)
			SetAxisBinding(name, keyAxis);
	}

	void InputBinding::AddAxisBinding(const String& name, ControllerAxis controllerAxis, bool setIfExist) {
		if (!HasAxisBinding(name))
			m_AxisBindings.emplace_back(name, KeyAxis{ KeyCode::None, KeyCode::None, KeyCode::None, KeyCode::None }, controllerAxis, false);
		else if (setIfExist)
			SetAxisBinding(name, controllerAxis);
	}

	void InputBinding::AddMouseAxisBinding(const String& name, bool setIfExist) {
		if (!HasAxisBinding(name))
			m_AxisBindings.emplace_back(name, KeyAxis{ KeyCode::None, KeyCode::None, KeyCode::None, KeyCode::None }, ControllerAxis::None, true);
		else if (setIfExist)
			SetMouseAxisBinding(name);
	}

	void InputBinding::RemoveMouseAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name) binding.mouseMovement = false;
	}
	
	const bool InputBinding::HasBinding(const String& name) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) return true;
		return false;
	}

	const bool InputBinding::HasAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name) return true;
		return false;
	}
	
	void InputBinding::SetBinding(const String& name, KeyCode newKeycode) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) {
				binding.Key = newKeycode;
				binding.MouseButton = MouseButton::None;
			}
	}
	
	void InputBinding::SetBinding(const String& name, MouseButton newMouseButton) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) {
				binding.MouseButton = newMouseButton;
				binding.Key = KeyCode::None;
			}
	}

	void InputBinding::SetBinding(const String& name, ControllerButton newControllerButton) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name)
				binding.ControllerButton = newControllerButton;
	}

	void InputBinding::SetAxisBinding(const String& name, KeyAxis keyAxis) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name)
				binding.keyAxis = keyAxis;
	}

	void InputBinding::SetAxisBinding(const String& name, ControllerAxis controllerAxis) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name)
				binding.controllerAxis = controllerAxis;
	}

	void InputBinding::SetMouseAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name) binding.mouseMovement = true;
	}
	
	KeyCode InputBinding::GetBinding(const String& name) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) return binding.Key;

		return KeyCode::None;
	}
	
	MouseButton InputBinding::GetMouseBinding(const String& name) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) return binding.MouseButton;

		return MouseButton::None;
	}

	ControllerButton InputBinding::GetControllerButtonBinding(const String& name) {
		for (auto& binding : m_Bindings)
			if (binding.Name == name) return binding.ControllerButton;

		return ControllerButton::None;
	}

	KeyAxis InputBinding::GetAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name) return binding.keyAxis;

		return KeyAxis{ KeyCode::None, KeyCode::None, KeyCode::None, KeyCode::None };
	}

	ControllerAxis InputBinding::GetControllerAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name) return binding.controllerAxis;
		return ControllerAxis::None;
	}

	const bool InputBinding::GetMouseAxisBinding(const String& name) {
		for (auto& binding : m_AxisBindings)
			if (binding.Name == name)
				return binding.mouseMovement;
		return false;
	}
	
	bool InputBinding::RemoveBinding(const String& name) {
		for (int i = 0; i < m_Bindings.size(); i++)
			if (m_Bindings[i].Name == name) {
				m_Bindings.erase(m_Bindings.begin() + i);
				return true;
			}
		return false;
	}

	bool InputBinding::RemoveAxisBinding(const String& name) {
		for (int i = 0; i < m_AxisBindings.size(); i++)
			if (m_AxisBindings[i].Name == name) {
				m_AxisBindings.erase(m_AxisBindings.begin() + i);
				return true;
			}
		return false;
	}
	
	void InputBinding::Serialize(const std::filesystem::path& keyboardBindingFilepath, const std::filesystem::path& controllerBindingFilepath) {
		Serialization::KeyValueSerialization keyboardOut(keyboardBindingFilepath);
		Serialization::KeyValueSerialization controllerOut(controllerBindingFilepath);
		for (auto& binding : m_Bindings) {
			if (binding.Name == "" || binding.Name == "\"\"") continue;

			if (binding.Key != KeyCode::None)
				keyboardOut.AddKeyValue(binding.Name, KeyCodeToString(binding.Key));
			else if (binding.MouseButton != MouseButton::None)
				keyboardOut.AddKeyValue(binding.Name, MouseButtonToString(binding.MouseButton));
			if (binding.ControllerButton != ControllerButton::None)
				controllerOut.AddKeyValue(binding.Name, ControllerButtonToString(binding.ControllerButton));
		}
		for (auto& binding : m_AxisBindings) {
			if (binding.Name == "" || binding.Name == "\"\"") continue;

			if (binding.controllerAxis != ControllerAxis::None)
				controllerOut.AddKeyValue("[Axis] " + binding.Name, ControllerAxisToString(binding.controllerAxis));
			if (binding.keyAxis.Up != KeyCode::None &&
				binding.keyAxis.Down != KeyCode::None &&
				binding.keyAxis.Left != KeyCode::None &&
				binding.keyAxis.Right != KeyCode::None)
				keyboardOut.AddKeyValue("[Axis] " + binding.Name, binding.keyAxis.ToString());
			else if (binding.mouseMovement)
				keyboardOut.AddKeyValue("[Axis] " + binding.Name, "MouseMovement");
		}
		keyboardOut.Save();
		controllerOut.Save();
	}
	
	void InputBinding::Deserize(const std::filesystem::path& keyboardBindingFilepath, const std::filesystem::path& controllerBindingFilepath) {
		if (!std::filesystem::exists(keyboardBindingFilepath) ||
			!std::filesystem::exists(controllerBindingFilepath)) Serialize(keyboardBindingFilepath, controllerBindingFilepath);

		Array<Serialization::KeyValueDeserialization::KeyValuePair> keyboardKeyValuePairs;
		Array<Serialization::KeyValueDeserialization::KeyValuePair> controllerKeyValuePairs;
		Serialization::KeyValueDeserialization::Deserialize(keyboardBindingFilepath, keyboardKeyValuePairs);
		Serialization::KeyValueDeserialization::Deserialize(controllerBindingFilepath, controllerKeyValuePairs);
		for (auto& pair : keyboardKeyValuePairs) {
			// Min sized string: [Axis] A    (8 chars)
			if (pair.Key.size() > 7 && pair.Key.substr(0, 7) == "[Axis] ") {
				String realName = pair.Key.substr(7, pair.Key.size() - 7);
				if (pair.Value == "MouseMovement")
					AddMouseAxisBinding(realName, true);
				else {
					KeyAxis axis(pair.Value);
					if (axis.Up != KeyCode::None &&
						axis.Down != KeyCode::None &&
						axis.Left != KeyCode::None &&
						axis.Right != KeyCode::None)
						AddAxisBinding(realName, axis, true);
				}
			}
			else {
				if (StringToKeyCode(pair.Value) != KeyCode::None)
					AddBinding(pair.Key, StringToKeyCode(pair.Value), true);
				else if (StringToMouseButton(pair.Value) != MouseButton::None)
					AddBinding(pair.Key, StringToMouseButton(pair.Value), true);
			}
		}
		for (auto& pair : controllerKeyValuePairs) {
			// Min sized string: [Axis] A    (8 chars)
			if (pair.Key.size() > 7 && pair.Key.substr(0, 7) == "[Axis] ") {
				String realName = pair.Key.substr(7, pair.Key.size() - 7);
				ControllerAxis axis = ControllerAxisFromString(pair.Value);
				if (axis != ControllerAxis::None)
					AddAxisBinding(realName, axis, true);
			}
			else if (ControllerButtonFromString(pair.Value) != ControllerButton::None)
				AddBinding(pair.Key, ControllerButtonFromString(pair.Value), true);
		}
	}
}