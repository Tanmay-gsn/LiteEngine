#pragma once

#include <memory>

#ifdef LE_PLATFORM_WINDOWS
#if LE_DYNAMIC_LINK
#ifdef LE_BUILD_DLL
#define LITEENGINE_API __declspec(dllexport)
#else
#define LITEENGINE_API __declspec(dllimport)
#endif
#else
#define LITEENGINE_API      // Static lib: no dllexport/import needed
#endif
#else
#error LiteEngine only supports Windows!
#endif

#ifdef LE_DEBUG
#define LE_ENABLE_ASSERTS
#endif

#ifdef LE_ENABLE_ASSERTS
#define LE_ASSERT(x, ...) { if(!(x)) { LE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LE_CORE_ASSERT(x, ...) { if(!(x)) { LE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define LE_ASSERT(x, ...)
#define LE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define LE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace LiteEngine {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}