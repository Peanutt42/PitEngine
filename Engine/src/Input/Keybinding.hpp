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
		void AddBinding(const String& name, KeyCode keycode, bool setIfExist = false);
		void AddBinding(const String& name, MouseButton mouseButton, bool setIfExist = false);
		
		bool HasBinding(const String& name);
		
		void SetBinding(const String& name, KeyCode newKeycode);
		void SetBinding(const String& name, MouseButton newMouseButton);
		
		KeyCode GetBinding(const String& name);
		MouseButton GetMouseBinding(const String& name);

		bool RemoveBinding(const String& name);

		void Serialize(const String& filepath);
		void Deserize(const String& filepath);

	private:
		Array<KeyBindingEntry> m_Bindings;
	};
}