#pragma once

#include <string>

namespace Pit {
	enum ResultType {
		Ok,
		Warning,
		Error
	};

	template<typename T>
	struct Result {
		inline bool IsOk() {
			return result == Ok;
		}
		
		inline bool IsSuboptimal() {
			return result == Warning;
		}
		inline bool IsFailed() {
			return result == Error;
		}
		ResultType result;
		std::string msg;
		T Value;
	};

	template<typename T>
	inline static Result<T> CreateOkResult(const T& Value) {
		Result<T> result(ResultType::Ok, "", Value);
		return result;
	}

	template<typename T>
	inline static Result<T> CreateSuboptimalResult(const T& Value, const std::string& msg) {
		Result<T> result(ResultType::Warning, msg, Value);
		return result;
	}

	template<typename T>
	inline static Result<T> CreateErrorResult(const T& Value, const std::string& msg) {
		Result<T> result(ResultType::Error, msg, Value);
		return result;
	}
}