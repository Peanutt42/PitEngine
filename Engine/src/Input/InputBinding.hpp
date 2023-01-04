#pragma once

#include "Core/CoreInclude.hpp"
#include "KeyCode.hpp"
#include "Serialization/KeyValueSerialization.hpp"

namespace Pit {
	struct ButtonBindingEntry {
		String Name;
		KeyCode Key = KeyCode::None;
		MouseButton MouseButton = MouseButton::None;
		ControllerButton ControllerButton = ControllerButton::None;
	};

	struct AxisBindingEntry {
		String Name;
		KeyAxis keyAxis;
		ControllerAxis controllerAxis;
		bool mouseMovement;
	};

	class InputBinding {
	public:
		void AddBinding(const String& name, KeyCode keycode, bool setIfExist = false);
		void AddBinding(const String& name, MouseButton mouseButton, bool setIfExist = false);
		void AddBinding(const String& name, ControllerButton controllerButton, bool setIfExist = false);

		void AddAxisBinding(const String& name, const KeyAxis& keyAxis, bool setIfExist = false);
		void AddAxisBinding(const String& name, ControllerAxis controllerAxis, bool setIfExist = false);
		void AddMouseAxisBinding(const String& name, bool setIfExist = false);

		const bool HasBinding(const String& name);
		const bool HasAxisBinding(const String& name);
		
		void SetBinding(const String& name, KeyCode newKeycode);
		void SetBinding(const String& name, MouseButton newMouseButton);
		void SetBinding(const String& name, ControllerButton newControllerButton);
		
		void SetAxisBinding(const String& name, KeyAxis keyAxis);
		void SetAxisBinding(const String& name, ControllerAxis controllerAxis);
		void SetMouseAxisBinding(const String& name);
		void RemoveMouseAxisBinding(const String& name);

		KeyCode GetBinding(const String& name);
		MouseButton GetMouseBinding(const String& name);
		ControllerButton GetControllerButtonBinding(const String& name);

		KeyAxis GetAxisBinding(const String& name);
		ControllerAxis GetControllerAxisBinding(const String& name);
		const bool GetMouseAxisBinding(const String& name);

		bool RemoveBinding(const String& name);
		bool RemoveAxisBinding(const String& name);

		void Serialize(const std::filesystem::path& keyboardBindingFilepath, const std::filesystem::path& controllerBindingFilepath);
		void Deserize(const std::filesystem::path& keyboardBindingFilepath, const std::filesystem::path& controllerBindingFilepath);

	private:
		Array<ButtonBindingEntry> m_Bindings;
		Array<AxisBindingEntry> m_AxisBindings;
	};
}