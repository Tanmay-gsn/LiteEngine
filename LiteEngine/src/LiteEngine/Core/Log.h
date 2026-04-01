#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace LiteEngine
{
	class LITEENGINE_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
// Core log Macros
#define LE_CORE_TRACE(...)	::LiteEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LE_CORE_INFO(...)	::LiteEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LE_CORE_WARN(...)	::LiteEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LE_CORE_ERROR(...)	::LiteEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LE_CORE_FATAL(...)	::LiteEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Cliet log Macros
#define LE_TRACE(...)		::LiteEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LE_INFO(...)		::LiteEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LE_WARN(...)		::LiteEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LE_ERROR(...)		::LiteEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LE_FATAL(...)		::LiteEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)