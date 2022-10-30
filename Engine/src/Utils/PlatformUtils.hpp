#pragma once

#include <string>

namespace Pit {
	class FileDialogs {
	public:
		static String OpenFile(const char* filter);
		static String SaveFile(const char* filter);
	};
}